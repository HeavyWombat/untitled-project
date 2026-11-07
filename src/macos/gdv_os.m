#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <OpenGL/OpenGL.h>
#import <mach/mach_time.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreText/CoreText.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gdv_os.h>

/* macOS Platform Layer Implementation */

int isFullscreen = 0;
static int windowWasInitialised = 0;

static NSWindow *gdv_os_window = nil;
static NSOpenGLView *gdv_os_glView = nil;
static NSOpenGLContext *gdv_os_glContext = nil;

static char* gdv_os_windowTitle = NULL;
static int gdv_os_windowWidth = 0;
static int gdv_os_windowHeight = 0;

static float gdv_os_width = 0;
static float gdv_os_height = 0;

static mach_timebase_info_data_t gdv_os_timebaseInfo;
static uint64_t gdv_os_startTime = 0;
static int gdv_os_timeWasInitialised = 0;
static int gdv_os_isPaused = 0;
static double gdv_os_currentTime = 0;

static int isFontInitialised = 0;

static int gdv_os_cursorHidden = 0;
static int gdv_os_mouseRelative = 0;
static double gdv_os_virtualMouseX = 0.0;
static double gdv_os_virtualMouseY = 0.0;

int (*gdv_os_mainloopFunction)(void) = NULL;
void (*gdv_os_windowEventFunction)(GDV_BaseEvent* event) = NULL;
void (*gdv_os_mouseEventFunction)(GDV_BaseEvent* event) = NULL;
void (*gdv_os_keyEventFunction)(GDV_BaseEvent* event) = NULL;

/* Forward declarations */
int gdv_os_openWindow_intern(char* title, int width, int height, int fullscreen);
int gdv_os_translateEventToCommonTable(NSEvent* event);

/* Convert a top-down view coordinate into CoreGraphics' global coordinate
   space. Window frames and screen heights are in points, so the conversion
   goes through Cocoa's point-based APIs; CGDisplayPixelsHigh returns device
   pixels, which put the warp target off target on Retina displays. */
static CGPoint gdv_os_viewPointToCGPoint(double x, double y) {
    NSPoint viewPoint = NSMakePoint((CGFloat)x, [gdv_os_glView bounds].size.height - (CGFloat)y);
    NSPoint windowPoint = [gdv_os_glView convertPoint:viewPoint toView:nil];
    NSPoint screenPoint = [gdv_os_window convertPointToScreen:windowPoint];
    CGFloat mainScreenTop = NSMaxY([[[NSScreen screens] objectAtIndex:0] frame]);

    return CGPointMake(screenPoint.x, mainScreenTop - screenPoint.y);
}

static void gdv_os_warpMouseToViewPoint(double x, double y) {
    if (!gdv_os_window || !gdv_os_glView) {
        return;
    }

    CGWarpMouseCursorPosition(gdv_os_viewPointToCGPoint(x, y));
}

/* Grab-style mouse mode: place the cursor in the middle of the view and then
   decouple it from the pointer, so the window server keeps delivering move
   events with delta data while the cursor itself stays put. Warping the
   cursor back after every single event (the previous approach) gets throttled
   by the window server, which made movement stutter and eventually stall. */
static void gdv_os_enterRelativeMode(void) {
    if (!gdv_os_window || !gdv_os_glView) {
        return;
    }

    gdv_os_virtualMouseX = [gdv_os_glView bounds].size.width / 2.0;
    gdv_os_virtualMouseY = [gdv_os_glView bounds].size.height / 2.0;

    gdv_os_warpMouseToViewPoint(gdv_os_virtualMouseX, gdv_os_virtualMouseY);

    CGError error = CGAssociateMouseAndMouseCursorPosition(false);
    gdv_os_mouseRelative = (error == kCGErrorSuccess);
}

static void gdv_os_leaveRelativeMode(void) {
    if (gdv_os_mouseRelative) {
        CGAssociateMouseAndMouseCursorPosition(true);
        gdv_os_mouseRelative = 0;
    }
}

static void gdv_os_sendMouseEvent(GDV_EventType eventType, NSPoint viewLocation, int wheelDelta) {
    if (gdv_os_mouseEventFunction == NULL) {
        return;
    }

    GDV_Event_Mouse me = {eventType, 0, 0, wheelDelta};

    if (gdv_os_mouseRelative) {
        me.xPos = (int)gdv_os_virtualMouseX;
        me.yPos = (int)gdv_os_virtualMouseY;
    } else {
        me.xPos = (int)viewLocation.x;
        me.yPos = (int)([gdv_os_glView bounds].size.height - viewLocation.y);
    }

    gdv_os_mouseEventFunction((GDV_BaseEvent*)&me);
}

/* Custom OpenGL View */
@interface GDVOpenGLView : NSOpenGLView
{
    NSTrackingArea *trackingArea;
}
@end

@implementation GDVOpenGLView

- (void)updateTrackingAreas {
    if (trackingArea) {
        [self removeTrackingArea:trackingArea];
    }

    trackingArea = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                options:(NSTrackingMouseMoved | NSTrackingActiveInKeyWindow)
                                                  owner:self
                                               userInfo:nil];
    [self addTrackingArea:trackingArea];
    [super updateTrackingAreas];
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)mouseDown:(NSEvent *)event {
    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    gdv_os_sendMouseEvent(GDV_EVENT_MOUSE_DOWN_LEFT_BUTTON, location, 0);
}

- (void)mouseUp:(NSEvent *)event {
    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    gdv_os_sendMouseEvent(GDV_EVENT_MOUSE_UP_LEFT_BUTTON, location, 0);
}

- (void)rightMouseDown:(NSEvent *)event {
    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    gdv_os_sendMouseEvent(GDV_EVENT_MOUSE_DOWN_RIGHT_BUTTON, location, 0);
}

- (void)rightMouseUp:(NSEvent *)event {
    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    gdv_os_sendMouseEvent(GDV_EVENT_MOUSE_UP_RIGHT_BUTTON, location, 0);
}

- (void)mouseMoved:(NSEvent *)event {
    /* In relative mode the cursor does not move; movement comes from the
       event deltas (y positive means downward, matching the top-down view
       coordinates the game expects). */
    if (gdv_os_mouseRelative) {
        gdv_os_virtualMouseX += [event deltaX];
        gdv_os_virtualMouseY += [event deltaY];
    }

    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    gdv_os_sendMouseEvent(GDV_EVENT_MOUSE_MOVE, location, 0);
}

- (void)mouseDragged:(NSEvent *)event {
    [self mouseMoved:event];
}

- (void)rightMouseDragged:(NSEvent *)event {
    [self mouseMoved:event];
}

- (void)scrollWheel:(NSEvent *)event {
    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    gdv_os_sendMouseEvent(GDV_EVENT_MOUSE_WHEEL, location, (int)([event deltaY] * 120));  // Convert to Windows-style delta
}

- (void)keyDown:(NSEvent *)event {
    if (gdv_os_keyEventFunction) {
        int translatedKey = gdv_os_translateEventToCommonTable(event);

        if (translatedKey == GDV_KEY_ESCAPE) {
            gdv_os_exit();
        } else {
            GDV_Event_Keyboard ke = {GDV_EVENT_KEY_DOWN, translatedKey};
            gdv_os_keyEventFunction((GDV_BaseEvent*)&ke);
        }
    }
}

- (void)keyUp:(NSEvent *)event {
    if (gdv_os_keyEventFunction) {
        int translatedKey = gdv_os_translateEventToCommonTable(event);

        GDV_Event_Keyboard ke = {GDV_EVENT_KEY_UP, translatedKey};
        gdv_os_keyEventFunction((GDV_BaseEvent*)&ke);
    }
}

@end

/* Custom Window Delegate */
@interface GDVWindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation GDVWindowDelegate

- (void)windowWillClose:(NSNotification *)notification {
    gdv_os_leaveRelativeMode();
    [NSApp terminate:nil];
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    /* Re-grab the mouse after regaining focus so look movement keeps flowing */
    if (gdv_os_cursorHidden) {
        gdv_os_enterRelativeMode();
    }
}

- (void)windowDidResize:(NSNotification *)notification {
    NSWindow *window = [notification object];
    NSRect frame = [[window contentView] bounds];

    gdv_os_width = frame.size.width;
    gdv_os_height = frame.size.height;

    if (gdv_os_windowEventFunction) {
        GDV_Event_Window_Size ws = {
            GDV_EVENT_WINDOW_SIZE,
            frame.size.width,
            frame.size.height
        };
        gdv_os_windowEventFunction((GDV_BaseEvent*)&ws);
    }
}

@end

static GDVWindowDelegate* gdv_os_windowDelegate = nil;

/* Implementation */

int gdv_os_init() {
    gdv_os_mainloopFunction = NULL;

    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    }

    return GDV_OK;
}

int gdv_os_openWindow(char* title, int width, int height) {
    isFullscreen = 0;
    windowWasInitialised = 1;

    if (gdv_os_windowTitle) free(gdv_os_windowTitle);
    gdv_os_windowTitle = (char*)malloc(strlen(title) + 1);
    strcpy(gdv_os_windowTitle, title);
    gdv_os_windowWidth = width;
    gdv_os_windowHeight = height;
    gdv_os_width = width;
    gdv_os_height = height;

    return gdv_os_openWindow_intern(title, width, height, 0);
}

int gdv_os_setFullscreen() {
    isFullscreen = 1;

    if (windowWasInitialised == 0) {
        return GDV_ERROR;
    }

    return gdv_os_openWindow_intern(gdv_os_windowTitle, gdv_os_windowWidth, gdv_os_windowHeight, 1);
}

int gdv_os_setWindowed() {
    isFullscreen = 0;

    if (windowWasInitialised == 0) {
        return GDV_ERROR;
    }

    return gdv_os_openWindow_intern(gdv_os_windowTitle, gdv_os_windowWidth, gdv_os_windowHeight, 0);
}

int gdv_os_toggleFullscreenWindow() {
    if (windowWasInitialised == 0) {
        return GDV_ERROR;
    }

    if (isFullscreen == 0) {
        isFullscreen = 1;
        return gdv_os_openWindow_intern(gdv_os_windowTitle, gdv_os_windowWidth, gdv_os_windowHeight, 1);
    } else {
        isFullscreen = 0;
        return gdv_os_openWindow_intern(gdv_os_windowTitle, gdv_os_windowWidth, gdv_os_windowHeight, 0);
    }
}

int gdv_os_openWindow_intern(char* title, int width, int height, int fullscreen) {
    @autoreleasepool {
        gdv_os_closeWindow();

        NSRect frame;
        NSWindowStyleMask style;

        if (fullscreen == 1) {
            frame = [[NSScreen mainScreen] frame];
            style = NSWindowStyleMaskBorderless;
        } else {
            frame = NSMakeRect(0, 0, width, height);
            style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
        }

        gdv_os_window = [[NSWindow alloc] initWithContentRect:frame
                                                     styleMask:style
                                                       backing:NSBackingStoreBuffered
                                                         defer:NO];

        if (!gdv_os_window) {
            return GDV_ERROR;
        }

        [gdv_os_window setTitle:[NSString stringWithUTF8String:title]];
        if (gdv_os_windowDelegate == nil) {
            gdv_os_windowDelegate = [[GDVWindowDelegate alloc] init];
        }
        [gdv_os_window setDelegate:gdv_os_windowDelegate];
        [gdv_os_window center];
        [gdv_os_window makeKeyAndOrderFront:nil];

        if (fullscreen == 1) {
            [gdv_os_window setLevel:NSMainMenuWindowLevel + 1];
        }

        /* Create OpenGL pixel format - no accumulation buffer for Metal compatibility */
        NSOpenGLPixelFormatAttribute attrs[] = {
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFAColorSize, 32,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFAStencilSize, 8,
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
            NSOpenGLPFAAccelerated,
            0
        };

        NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
        if (!pixelFormat) {
            NSLog(@"Failed to create accelerated pixel format, trying software fallback");
            NSOpenGLPixelFormatAttribute fallbackAttrs[] = {
                NSOpenGLPFADoubleBuffer,
                NSOpenGLPFAColorSize, 32,
                NSOpenGLPFAAlphaSize, 8,
                NSOpenGLPFADepthSize, 16,
                NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
                0
            };
            pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:fallbackAttrs];
            if (!pixelFormat) {
                NSLog(@"Failed to create any pixel format");
                return GDV_ERROR;
            }
        }

        /* Create OpenGL view */
        gdv_os_glView = [[GDVOpenGLView alloc] initWithFrame:frame pixelFormat:pixelFormat];
        if (!gdv_os_glView) {
            return GDV_ERROR;
        }

        [gdv_os_window setContentView:gdv_os_glView];

        /* Let the game know the new window size (the window may have been
           recreated in a different size, e.g. by a fullscreen toggle) */
        {
            NSRect viewBounds = [gdv_os_glView bounds];

            gdv_os_width = viewBounds.size.width;
            gdv_os_height = viewBounds.size.height;

            if (gdv_os_windowEventFunction) {
                GDV_Event_Window_Size ws = {
                    GDV_EVENT_WINDOW_SIZE,
                    viewBounds.size.width,
                    viewBounds.size.height
                };
                gdv_os_windowEventFunction((GDV_BaseEvent*)&ws);
            }
        }

        /* Use a 1x OpenGL surface so the viewport (set in points) matches the
           actual framebuffer. Without this, Retina displays create a 2x
           surface while the game still uses point dimensions, causing the
           scene to render in only the bottom-left quarter of the window. */
        [gdv_os_glView setWantsBestResolutionOpenGLSurface:NO];

        gdv_os_glContext = [gdv_os_glView openGLContext];
        [gdv_os_glContext makeCurrentContext];

        /* Enable vsync */
        GLint swapInt = 1;
        [gdv_os_glContext setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

        /* Make sure context is fully current and ready */
        [[gdv_os_glView openGLContext] makeCurrentContext];
        glFlush();

        [gdv_os_window makeFirstResponder:gdv_os_glView];
        [NSApp activateIgnoringOtherApps:YES];

        /* Re-grab mouse look if the window was recreated while the cursor
           is hidden (e.g. fullscreen toggle) */
        if (gdv_os_cursorHidden) {
            gdv_os_enterRelativeMode();
        }

        /* Initialize font rendering placeholder */
        if (isFontInitialised == 0) {
            isFontInitialised = 1;
            /* TODO: Implement text rendering with CoreText or FreeType */
        }
    }

    return GDV_OK;
}

int gdv_os_closeWindow() {
    @autoreleasepool {
        gdv_os_leaveRelativeMode();

        if (gdv_os_glContext) {
            [NSOpenGLContext clearCurrentContext];
            gdv_os_glContext = nil;
        }

        if (gdv_os_window) {
            /* Detach the delegate so that closing the window as part of an
               internal recreation (e.g. fullscreen toggle) does not trigger
               the delegate's terminate-on-close behavior, which would quit
               the whole application. */
            [gdv_os_window setDelegate:nil];
            [gdv_os_window close];
            gdv_os_window = nil;
        }

        gdv_os_glView = nil;
    }

    return GDV_OK;
}

void gdv_os_registerMainloopFunction(int (*mainloopFunction)(void)) {
    gdv_os_mainloopFunction = mainloopFunction;
}

void gdv_os_registerWindowEventFunction(void (*windowEventFunction)(GDV_BaseEvent* event)) {
    gdv_os_windowEventFunction = windowEventFunction;
}

void gdv_os_registerMouseEventFunction(void (*mouseEventFunction)(GDV_BaseEvent* event)) {
    gdv_os_mouseEventFunction = mouseEventFunction;
}

void gdv_os_registerKeyEventFunction(void (*keyEventFunction)(GDV_BaseEvent* event)) {
    gdv_os_keyEventFunction = keyEventFunction;
}

int gdv_os_runMainloop() {
    if (gdv_os_mainloopFunction == NULL) {
        return GDV_ERROR;
    }

    @autoreleasepool {
        [NSApp finishLaunching];

        char stop = 0;
        while (stop != 1) {
            @autoreleasepool {
                /* Ensure OpenGL context is current */
                [gdv_os_glContext makeCurrentContext];

                /* Process events */
                NSEvent *event;
                while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                                   untilDate:[NSDate distantPast]
                                                      inMode:NSDefaultRunLoopMode
                                                     dequeue:YES])) {
                    [NSApp sendEvent:event];
                }

                /* Run game logic */
                stop = (*gdv_os_mainloopFunction)();

                /* Update and swap buffers */
                [gdv_os_glContext update];
                [gdv_os_glContext flushBuffer];

                /* Force window update */
                [[gdv_os_glView window] displayIfNeeded];
            }
        }

        gdv_os_leaveRelativeMode();
    }

    return GDV_OK;
}

void gdv_os_getWindowSize(float* width, float* height) {
    *width = gdv_os_width;
    *height = gdv_os_height;
}

double gdv_os_getCurrentTime() {
    if (gdv_os_isPaused == 0) {
        if (gdv_os_timeWasInitialised == 0) {
            mach_timebase_info(&gdv_os_timebaseInfo);
            gdv_os_startTime = mach_absolute_time();
            gdv_os_timeWasInitialised = 1;
        }

        uint64_t currentTime = mach_absolute_time();
        uint64_t elapsed = currentTime - gdv_os_startTime;
        gdv_os_currentTime = (double)elapsed * gdv_os_timebaseInfo.numer / gdv_os_timebaseInfo.denom / 1000000000.0;
    }

    return gdv_os_currentTime;
}

void gdv_os_toggleIsPause() {
    if (gdv_os_isPaused == 0) {
        gdv_os_isPaused = 1;
    } else {
        gdv_os_isPaused = 0;
    }
}

void gdv_os_setWindowTitle(char* title) {
    @autoreleasepool {
        if (gdv_os_window) {
            [gdv_os_window setTitle:[NSString stringWithUTF8String:title]];
        }
    }
}

void gdv_os_showCursor(int visible) {
    @autoreleasepool {
        gdv_os_cursorHidden = (visible == 0) ? 1 : 0;

        if (visible == 1) {
            gdv_os_leaveRelativeMode();
            [NSCursor unhide];
        } else {
            [NSCursor hide];
            gdv_os_enterRelativeMode();
        }
    }
}

void gdv_os_exit() {
    gdv_os_leaveRelativeMode();
    [NSApp terminate:nil];
}

int gdv_os_setMousePosition(long x, long y, int absoluteRelativeFlag, long* resultX, long* resultY) {
    @autoreleasepool {
        if (absoluteRelativeFlag == 0) {
            /* Absolute positioning */
            if (!gdv_os_window || !gdv_os_glView) {
                return GDV_ERROR;
            }

            if (gdv_os_mouseRelative) {
                /* The cursor is decoupled from the pointer; only re-anchor the
                   virtual position so the next event's delta starts from here. */
                gdv_os_virtualMouseX = (double)x;
                gdv_os_virtualMouseY = (double)y;
            } else {
                gdv_os_warpMouseToViewPoint((double)x, (double)y);
            }

            if (resultX) *resultX = x;
            if (resultY) *resultY = y;
        } else {
            /* Relative positioning - not fully implemented */
            if (resultX) *resultX = x;
            if (resultY) *resultY = y;
        }
    }

    return GDV_OK;
}

void gdv_os_drawOutlinedText(const char *text, ...) {
    /* TODO: Implement with CoreText or FreeType */
    /* For now, this is a stub */
}

static CTFontRef gdv_os_textFont = NULL;

static CTFontRef gdv_os_getTextFont(void) {
    if (gdv_os_textFont == NULL) {
        /* Same face and height as the Windows implementation */
        gdv_os_textFont = CTFontCreateWithName(CFSTR("Courier New"), 12.0, NULL);
    }

    return gdv_os_textFont;
}

static void gdv_os_drawTextLine(const char* line, size_t length, float x, float baselineY) {
    CFStringRef string;
    CFStringRef attributeKeys[2];
    CFTypeRef attributeValues[2];
    CFDictionaryRef attributes;
    CFAttributedStringRef attributedString;
    CTLineRef textLine;
    CGColorSpaceRef colorSpace;
    CGColorRef textColor;
    CGContextRef colorContext;
    const CGFloat whiteComponents[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    CGFloat ascent = 0.0f;
    CGFloat descent = 0.0f;
    CGFloat leading = 0.0f;
    double lineWidth;
    size_t imageWidth;
    size_t imageHeight;
    size_t rowBytes;
    unsigned char* pixels;
    GLint unpackAlignment;

    if (length == 0) {
        return;
    }

    string = CFStringCreateWithBytes(NULL, (const UInt8*)line, (CFIndex)length, kCFStringEncodingUTF8, false);
    if (string == NULL) {
        /* Fall back to Latin-1 for strings that are not valid UTF-8 */
        string = CFStringCreateWithBytes(NULL, (const UInt8*)line, (CFIndex)length, kCFStringEncodingISOLatin1, false);
    }
    if (string == NULL) {
        return;
    }

    /* CoreText draws glyphs in black unless a foreground color attribute is
       present (it does not use the context fill color by default). */
    colorSpace = CGColorSpaceCreateDeviceRGB();
    textColor = CGColorCreate(colorSpace, whiteComponents);

    attributeKeys[0] = kCTFontAttributeName;
    attributeValues[0] = gdv_os_getTextFont();
    attributeKeys[1] = kCTForegroundColorAttributeName;
    attributeValues[1] = textColor;
    attributes = CFDictionaryCreate(NULL, (const void**)attributeKeys, (const void**)attributeValues, 2,
                                    &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    attributedString = CFAttributedStringCreate(NULL, string, attributes);
    CFRelease(attributes);
    CFRelease(string);

    if (attributedString == NULL) {
        CFRelease(textColor);
        CGColorSpaceRelease(colorSpace);
        return;
    }

    textLine = CTLineCreateWithAttributedString(attributedString);
    CFRelease(attributedString);
    CFRelease(textColor);

    if (textLine == NULL) {
        CGColorSpaceRelease(colorSpace);
        return;
    }

    lineWidth = CTLineGetTypographicBounds(textLine, &ascent, &descent, &leading);

    imageWidth = (size_t)ceil(lineWidth) + 2;
    imageHeight = (size_t)ceil(ascent + descent) + 2;
    rowBytes = imageWidth * 4;

    if ((imageWidth == 0) || (imageHeight == 0) || (imageWidth > 1024) || (imageHeight > 64)) {
        CFRelease(textLine);
        CGColorSpaceRelease(colorSpace);
        return;
    }

    pixels = (unsigned char*)calloc(rowBytes * imageHeight, 1);
    if (pixels == NULL) {
        CFRelease(textLine);
        CGColorSpaceRelease(colorSpace);
        return;
    }

    colorContext = CGBitmapContextCreate(pixels, imageWidth, imageHeight, 8, rowBytes, colorSpace, kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colorSpace);

    if (colorContext == NULL) {
        free(pixels);
        CFRelease(textLine);
        return;
    }

    /* White antialiased text on the (calloc'd) transparent background.
       Premultiplied RGBA is uploaded with a GL_ONE blend, so the coverage
       ends up in the framebuffer correctly. A GL_ALPHA upload would be
       silently dropped by the GL-on-Metal driver. */
    CGContextSetTextMatrix(colorContext, CGAffineTransformIdentity);
    CGContextSetTextPosition(colorContext, 0.0f, descent + 1.0f);
    CTLineDraw(textLine, colorContext);

    CGContextRelease(colorContext);
    CFRelease(textLine);

    // ---

    /* Draw at the current raster position (window coordinates), keeping the
       baseline there like the Windows bitmap font does. */
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpackAlignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    /* Bitmap memory is top-down, so flip it while drawing */
    glPixelZoom(1.0f, -1.0f);
    glWindowPos2f(x, baselineY + (float)imageHeight - (float)descent - 1.0f);
    glDrawPixels((GLsizei)imageWidth, (GLsizei)imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glPixelZoom(1.0f, 1.0f);

    glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment);
    glPopAttrib();

    free(pixels);
}

void gdv_os_drawBitmappedText(const char *text, ...) {
    char expandedText[512];
    const char* lineStart;
    va_list ap;
    GLboolean rasterPositionValid = GL_TRUE;
    GLfloat rasterPosition[4];
    float x;
    float y;

    if (text == NULL) {
        return;
    }

    if (gdv_os_getTextFont() == NULL) {
        return;
    }

    va_start(ap, text);
    vsnprintf(expandedText, sizeof(expandedText), text, ap);
    va_end(ap);

    glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &rasterPositionValid);
    if (rasterPositionValid != GL_TRUE) {
        return;
    }

    glGetFloatv(GL_CURRENT_RASTER_POSITION, rasterPosition);

    x = rasterPosition[0];
    y = rasterPosition[1];

    lineStart = expandedText;
    for (;;) {
        const char* lineEnd = strchr(lineStart, '\n');
        size_t lineLength = (lineEnd != NULL) ? (size_t)(lineEnd - lineStart) : strlen(lineStart);

        gdv_os_drawTextLine(lineStart, lineLength, x, y);

        if (lineEnd == NULL) {
            break;
        }

        lineStart = lineEnd + 1;
        y -= 14.0f;  /* Same line step as the Windows implementation */
    }
}

int gdv_os_createScreenshot() {
    static char srccount1 = '0';
    static char srccount2 = '0';
    static char srccount3 = '0';

    int imageWidth = (int)gdv_os_width;
    int imageHeight = (int)gdv_os_height;

    char fileName[19];
    fileName[0] = 'S';
    fileName[1] = 'c';
    fileName[2] = 'r';
    fileName[3] = 'e';
    fileName[4] = 'e';
    fileName[5] = 'n';
    fileName[6] = 's';
    fileName[7] = 'h';
    fileName[8] = 'o';
    fileName[9] = 't';
    fileName[10] = '_';
    fileName[11] = srccount3;
    fileName[12] = srccount2;
    fileName[13] = srccount1;
    fileName[14] = '.';
    fileName[15] = 't';
    fileName[16] = 'g';
    fileName[17] = 'a';
    fileName[18] = '\0';

    srccount1++;
    if(srccount1 > '9') {
        srccount1 = '0';
        srccount2++;
        if(srccount2 > '9') {
            srccount2 = '0';
            srccount3++;
        }
    }

    char tgaHeader[18];
    char* imageBuffer = (char*)malloc(imageWidth * imageHeight * 3);
    int i;

    FILE* outputFile = fopen(fileName, "wb");

    memset(tgaHeader, 0, 18);
    tgaHeader[2] = 2;
    tgaHeader[12] = imageWidth & 255;
    tgaHeader[13] = imageWidth >> 8;
    tgaHeader[14] = imageHeight & 255;
    tgaHeader[15] = imageHeight >> 8;
    tgaHeader[16] = 24;

    glReadPixels(0, 0, imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, imageBuffer);

    for(i = 0; i < imageWidth * imageHeight * 3; i+=3) {
        unsigned char temp = imageBuffer[i];
        imageBuffer[i] = imageBuffer[i+2];
        imageBuffer[i+2] = temp;
    }

    fwrite(tgaHeader, sizeof(unsigned char), 18, outputFile);
    fwrite(imageBuffer, sizeof(unsigned char), imageWidth * imageHeight * 3, outputFile);

    fclose(outputFile);
    free(imageBuffer);

    return 0;
}

int gdv_os_translateEventToCommonTable(NSEvent* event) {
    /* macOS key code to common key code mapping */
    switch([event keyCode]) {
        case 0x35: return GDV_KEY_ESCAPE;
        case 0x7E: return GDV_KEY_UP;
        case 0x7D: return GDV_KEY_DOWN;
        case 0x7B: return GDV_KEY_LEFT;
        case 0x7C: return GDV_KEY_RIGHT;
        case 0x24: return GDV_KEY_ENTER;
        case 0x30: return GDV_KEY_TAB;
        case 0x31: return GDV_KEY_SPACE;
        case 0x33: return GDV_KEY_DELETE;
        case 0x75: return GDV_KEY_DELETE;  /* Forward delete */
        case 0x73: return GDV_KEY_HOME;
        case 0x77: return GDV_KEY_END;
        case 0x74: return GDV_KEY_PAGE_UP;
        case 0x79: return GDV_KEY_PAGE_DOWN;
        case 0x7A: return GDV_KEY_F1;
        case 0x78: return GDV_KEY_F2;
        case 0x63: return GDV_KEY_F3;
        case 0x76: return GDV_KEY_F4;
        case 0x60: return GDV_KEY_F5;
        case 0x61: return GDV_KEY_F6;
        case 0x62: return GDV_KEY_F7;
        case 0x64: return GDV_KEY_F8;
        case 0x65: return GDV_KEY_F9;
        case 0x6D: return GDV_KEY_F10;
        case 0x67: return GDV_KEY_F11;
        case 0x6F: return GDV_KEY_F12;
        case 0x38: /* Left Shift */
        case 0x3C: /* Right Shift */
            return GDV_KEY_SHIFT;
        case 0x3B: /* Left Control */
            return GDV_KEY_LEFT_CONTROL;
        case 0x3E: /* Right Control */
            return GDV_KEY_RIGHT_CONTROL;
        case 0x3A: /* Left Alt/Option */
        case 0x3D: /* Right Alt/Option */
            return GDV_KEY_ALT;
        case 0x39: return GDV_KEY_CAPS_LOCK;
        case 0x47: return GDV_KEY_CLEAR;
        case 0x71: return GDV_KEY_NUM_LOCK;
        case 0x72: return GDV_KEY_INSERT;
        case 0x6B: return GDV_KEY_PRINT_SCREEN;
        /* PAUSE key: macOS doesn't have a direct equivalent to 0x71 */
        case 0x6E: return GDV_KEY_SCROLL;
        default:
            break;
    }

    /* Printable keys: the game indexes gdv_key_states[] by ASCII character
       (e.g. 'w', 'a', 's', 'd'), so translate the character produced by the
       keyboard layout and lowercase letters, matching the Windows platform
       layer which uses ToAscii(). */
    NSString* characters = [event charactersIgnoringModifiers];

    if ([characters length] > 0) {
        unichar character = [characters characterAtIndex:0];

        if ((character >= 'A') && (character <= 'Z')) {
            character = character - 'A' + 'a';
        }

        return (int)character;
    }

    return (int)[event keyCode];
}
