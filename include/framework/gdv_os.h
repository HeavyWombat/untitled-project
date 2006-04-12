#ifndef __GDV_OS_H
#define __GDV_OS_H

#define GDV_OS_TIMEOUT 30000L
#define GDV_ERROR 1
#define GDV_OK 0

extern int isFullscreen;

typedef enum
{
	GDV_KEY_BACKSPACE = 0x08,
	GDV_KEY_TAB = 0x09,
	GDV_KEY_ENTER = 0x0A,
	GDV_KEY_ESCAPE = 0x1B,
	GDV_KEY_SPACE = 0x20,
	GDV_KEY_DELETE = 0x7F,
	GDV_KEY_F1 = 256 + 0,
	GDV_KEY_F2 = 256 + 1,
	GDV_KEY_F3 = 256 + 2,
	GDV_KEY_F4 = 256 + 3,
	GDV_KEY_F5 = 256 + 4,
	GDV_KEY_F6 = 256 + 5,
	GDV_KEY_F7 = 256 + 6,
	GDV_KEY_F8 = 256 + 7,
	GDV_KEY_F9 = 256 + 8,
	GDV_KEY_F10 = 256 + 9,
	GDV_KEY_F11 = 256 + 10,
	GDV_KEY_F12 = 256 + 11,
	GDV_KEY_PAGE_UP = 256 + 12,
	GDV_KEY_PAGE_DOWN = 256 + 13,
	GDV_KEY_SHIFT = 256 + 14,
	GDV_KEY_LEFT_CONTROL = 256 + 15,
	GDV_KEY_RIGHT_CONTROL = 256 + 16,
	GDV_KEY_ALT = 256 + 17,
	GDV_KEY_CLEAR = 256 + 18,
	GDV_KEY_PAUSE = 256 + 19,
	GDV_KEY_CAPS_LOCK = 256 + 20,
	GDV_KEY_END = 256 + 21,
	GDV_KEY_HOME = 256 + 22,
	GDV_KEY_LEFT = 256 + 23,
	GDV_KEY_UP = 256 + 24,
	GDV_KEY_RIGHT = 256 + 25,
	GDV_KEY_DOWN = 256 + 26,
	GDV_KEY_PRINT_SCREEN = 256 + 27,
	GDV_KEY_INSERT = 256 + 28,
	GDV_KEY_NUM_LOCK = 256 + 29,
	GDV_KEY_SCROLL = 256 + 30
} GDV_KeyCode;

typedef enum
{
	GDV_EVENT_WINDOW_SIZE,
	GDV_EVENT_MOUSE_MOVE,
	GDV_EVENT_MOUSE_DOUBLE_CLICK_LEFT_BUTTON,
	GDV_EVENT_MOUSE_DOWN_LEFT_BUTTON,
	GDV_EVENT_MOUSE_UP_LEFT_BUTTON,
	GDV_EVENT_MOUSE_DOUBLE_CLICK_RIGHT_BUTTON,
	GDV_EVENT_MOUSE_DOWN_RIGHT_BUTTON,
	GDV_EVENT_MOUSE_UP_RIGHT_BUTTON,
	GDV_EVENT_MOUSE_DOUBLE_CLICK_MIDDLE_BUTTON,
	GDV_EVENT_MOUSE_DOWN_MIDDLE_BUTTON,
	GDV_EVENT_MOUSE_UP_MIDDLE_BUTTON,
	GDV_EVENT_MOUSE_WHEEL,
	GDV_EVENT_KEY_DOWN,
	GDV_EVENT_KEY_UP,
	GDV_EVENT_NOT_DEFINED
} GDV_EventType;

typedef struct
{
	GDV_EventType eventType;
	char dataBuffer[24];
} GDV_BaseEvent;

typedef struct
{
	GDV_EventType eventType;
	float width;
	float height;
} GDV_Event_Window_Size;

typedef struct
{
	GDV_EventType eventType;
	int xPos;
	int yPos;
	int mouseWheelDelta;
} GDV_Event_Mouse;

typedef struct
{
	GDV_EventType eventType;
	int key;
} GDV_Event_Keyboard;

int gdv_os_init();
int gdv_os_openWindow(char* title, int width, int height);
void gdv_os_registerMainloopFunction(int (*mainloopFunction)(void));
void gdv_os_registerWindowEventFunction(void (*windowEventFunction)(GDV_BaseEvent* event));
void gdv_os_registerMouseEventFunction(void (*mouseEventFunction)(GDV_BaseEvent* event));
void gdv_os_registerKeyEventFunction(void (*keyEventFunction)(GDV_BaseEvent* event));
int gdv_os_runMainloop();
double gdv_os_getCurrentTime();
void gdv_os_toggleIsPause();
int gdv_os_setFullscreen();
int gdv_os_setWindowed();
int gdv_os_toggleFullscreenWindow();
int gdv_os_closeWindow();
void gdv_os_getWindowSize(float* width, float* height);
void gdv_os_setWindowTitle(char* title);
void gdv_os_showCursor(int visible);
void gdv_os_exit();
int gdv_os_setMousePosition(long x, long y, int absoluteRelativeFlag, long* resultX, long* resultY);
void gdv_os_drawOutlinedText(const char *text, ...);
void gdv_os_drawBitmappedText(const char *text, ...);
int gdv_os_createScreenshot();

#endif

