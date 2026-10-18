#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <OpenGL/OpenGL.h>
#import <mach/mach_time.h>
#import <CoreGraphics/CoreGraphics.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int (*gdv_os_mainloopFunction)(void) = NULL;
void (*gdv_os_windowEventFunction)(GDV_BaseEvent* event) = NULL;
void (*gdv_os_mouseEventFunction)(GDV_BaseEvent* event) = NULL;
void (*gdv_os_keyEventFunction)(GDV_BaseEvent* event) = NULL;

/* Forward declarations */
int gdv_os_openWindow_intern(char* title, int width, int height, int fullscreen);
int gdv_os_translateKeysToCommonTable(int key);

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
    if (gdv_os_mouseEventFunction) {
        NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
        GDV_Event_Mouse me = {
            GDV_EVENT_MOUSE_DOWN_LEFT_BUTTON,
            (int)location.x,
            (int)([self bounds].size.height - location.y),
            0
        };
        gdv_os_mouseEventFunction((GDV_BaseEvent*)&me);
    }
}

- (void)mouseUp:(NSEvent *)event {
    if (gdv_os_mouseEventFunction) {
        NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
        GDV_Event_Mouse me = {
            GDV_EVENT_MOUSE_UP_LEFT_BUTTON,
            (int)location.x,
            (int)([self bounds].size.height - location.y),
            0
        };
        gdv_os_mouseEventFunction((GDV_BaseEvent*)&me);
    }
}

- (void)rightMouseDown:(NSEvent *)event {
    if (gdv_os_mouseEventFunction) {
        NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
        GDV_Event_Mouse me = {
            GDV_EVENT_MOUSE_DOWN_RIGHT_BUTTON,
            (int)location.x,
            (int)([self bounds].size.height - location.y),
            0
        };
        gdv_os_mouseEventFunction((GDV_BaseEvent*)&me);
    }
}

- (void)rightMouseUp:(NSEvent *)event {
    if (gdv_os_mouseEventFunction) {
        NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
        GDV_Event_Mouse me = {
            GDV_EVENT_MOUSE_UP_RIGHT_BUTTON,
            (int)location.x,
            (int)([self bounds].size.height - location.y),
            0
        };
        gdv_os_mouseEventFunction((GDV_BaseEvent*)&me);
    }
}

- (void)mouseMoved:(NSEvent *)event {
    if (gdv_os_mouseEventFunction) {
        NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
        GDV_Event_Mouse me = {
            GDV_EVENT_MOUSE_MOVE,
            (int)location.x,
            (int)([self bounds].size.height - location.y),
            0
        };
        gdv_os_mouseEventFunction((GDV_BaseEvent*)&me);
    }
}

- (void)mouseDragged:(NSEvent *)event {
    [self mouseMoved:event];
}

- (void)rightMouseDragged:(NSEvent *)event {
    [self mouseMoved:event];
}

- (void)scrollWheel:(NSEvent *)event {
    if (gdv_os_mouseEventFunction) {
        NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
        GDV_Event_Mouse me = {
            GDV_EVENT_MOUSE_WHEEL,
            (int)location.x,
            (int)([self bounds].size.height - location.y),
            (int)([event deltaY] * 120)  // Convert to Windows-style delta
        };
        gdv_os_mouseEventFunction((GDV_BaseEvent*)&me);
    }
}

- (void)keyDown:(NSEvent *)event {
    if (gdv_os_keyEventFunction) {
        unsigned short keyCode = [event keyCode];
        int translatedKey = gdv_os_translateKeysToCommonTable(keyCode);

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
        unsigned short keyCode = [event keyCode];
        int translatedKey = gdv_os_translateKeysToCommonTable(keyCode);

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
    [NSApp terminate:nil];
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
        [gdv_os_window setDelegate:[[GDVWindowDelegate alloc] init]];
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
        if (gdv_os_glContext) {
            [NSOpenGLContext clearCurrentContext];
            gdv_os_glContext = nil;
        }

        if (gdv_os_window) {
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
        if (visible == 1) {
            [NSCursor unhide];
        } else {
            [NSCursor hide];
        }
    }
}

void gdv_os_exit() {
    [NSApp terminate:nil];
}

int gdv_os_setMousePosition(long x, long y, int absoluteRelativeFlag, long* resultX, long* resultY) {
    @autoreleasepool {
        if (absoluteRelativeFlag == 0) {
            /* Absolute positioning */
            if (!gdv_os_window) {
                return GDV_ERROR;
            }

            NSRect windowFrame = [gdv_os_window frame];
            CGPoint point = CGPointMake(windowFrame.origin.x + x,
                                       CGDisplayPixelsHigh(kCGDirectMainDisplay) - (windowFrame.origin.y + windowFrame.size.height - y));
            CGWarpMouseCursorPosition(point);

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

void gdv_os_drawBitmappedText(const char *text, ...) {
    /* TODO: Implement with CoreText or FreeType */
    /* For now, this is a stub */
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

int gdv_os_translateKeysToCommonTable(int key) {
    /* macOS key code to common key code mapping */
    switch(key) {
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
            /* Try to map alphanumeric keys */
            if (key >= 0 && key <= 127) {
                /* Basic ASCII mapping - will need refinement */
                return key;
            }
            return key;
    }
}
