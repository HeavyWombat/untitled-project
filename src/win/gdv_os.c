#include<GL/gl.h>
#include<GL/glu.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>
#include<process.h>
#include <direct.h>

#include<gdv_os.h>

static int isFullscreen = 0;
static int windowWasInitialised = 0;

static HINSTANCE gdv_os_programmInstance = NULL;
static HWND gdv_os_windowInstance = NULL;
static HDC gdv_os_deviceContext = NULL;
static HGLRC gdv_os_renderingContext = NULL;

static char gdv_os_applicationName[] = "GDV Programm";

static LARGE_INTEGER gdv_os_timeBaseFrequency;
static int gdv_os_timeWasInitialised = 0;
static int gdv_os_isPaused = 0;
static double gdv_os_currentTime = 0;

static char* gdv_os_windowTitle = NULL;
static int gdv_os_windowWidth = 0;
static int gdv_os_windowHeight = 0;

static float gdv_os_width = 0;
static float gdv_os_height = 0;

LONG WINAPI gdv_os_messageDispatchProcedure(HWND, UINT, WPARAM, LPARAM);
int gdv_os_openWindow_intern(char* title, int width, int height, int fullscreen);
int gdv_os_translateKeysToCommenTable(int key);

int (*gdv_os_mainloopFunction)(void) = NULL;
void (*gdv_os_windowEventFunction)(GDV_BaseEvent* event) = NULL;
void (*gdv_os_mouseEventFunction)(GDV_BaseEvent* event) = NULL;
void (*gdv_os_keyEventFunction)(GDV_BaseEvent* event) = NULL;

int gdv_os_init()
{
	gdv_os_mainloopFunction = NULL;

	gdv_os_programmInstance = GetModuleHandle(NULL);

	if(gdv_os_programmInstance == 0)
	{
		return(GDV_ERROR);
	}

	WNDCLASS myWindowClass;
	memset(&myWindowClass, 0, sizeof(WNDCLASS));
	
	myWindowClass.style = 0;
	myWindowClass.hInstance = gdv_os_programmInstance;
	myWindowClass.lpfnWndProc = (WNDPROC)gdv_os_messageDispatchProcedure;
	myWindowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	myWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	myWindowClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 51));
	myWindowClass.lpszMenuName = gdv_os_applicationName;
	myWindowClass.lpszClassName = gdv_os_applicationName;

	if(RegisterClass(&myWindowClass) == 0)
	{
		//erc = GetLastError()
		//FormatMessage(erc ...)

		return(GDV_ERROR);
	}

	return(GDV_OK);
}

int gdv_os_openWindow(char* title, int width, int height)
{
	int rc = GDV_OK;
	isFullscreen = 0;
	windowWasInitialised = 1;
	
	gdv_os_windowTitle = malloc(strlen(title) + 1);
	strcpy(gdv_os_windowTitle, title);
	gdv_os_windowWidth = width;
	gdv_os_windowHeight = height;
	gdv_os_width = width;
	gdv_os_height = height;

	rc = gdv_os_openWindow_intern(title, width, height, 0);
	
	return(rc);
}

int gdv_os_setFullscreen()
{
	int rc = GDV_OK;
	isFullscreen = 1;
	
	if(windowWasInitialised == 0)
	{
		return(GDV_ERROR);
	}

	rc = gdv_os_openWindow_intern(gdv_os_windowTitle, gdv_os_windowWidth, gdv_os_windowHeight, 1);
	
	return(rc);
}

int gdv_os_setWindowed()
{
	int rc = GDV_OK;
	isFullscreen = 0;
	
	if(windowWasInitialised == 0)
	{
		return(GDV_ERROR);
	}

	rc = gdv_os_openWindow_intern(gdv_os_windowTitle, gdv_os_windowWidth, gdv_os_windowHeight, 0);
	
	return(rc);
}

int gdv_os_toggleFullscreenWindow()
{
	int rc = GDV_OK;
	
	if(windowWasInitialised == 0)
	{
		return(GDV_ERROR);
	}

	if(isFullscreen == 0)
	{
		isFullscreen = 1;
		rc = gdv_os_openWindow_intern(gdv_os_windowTitle, gdv_os_windowWidth, gdv_os_windowHeight, 1);
	}
	else
	{
		isFullscreen = 0;
		rc = gdv_os_openWindow_intern(gdv_os_windowTitle, gdv_os_windowWidth, gdv_os_windowHeight, 0);
	}
	
	return(rc);
}

int gdv_os_openWindow_intern(char* title, int width, int height, int fullscreen)
{
	gdv_os_closeWindow();

	DWORD windowType;

	if(fullscreen == 1)
	{
		windowType = WS_POPUP;
	}
	else
	{
		windowType = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}

	gdv_os_windowInstance = CreateWindow(gdv_os_applicationName, title, windowType, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, gdv_os_programmInstance, NULL);

	if(gdv_os_windowInstance == 0)
	{
		return(GDV_ERROR);
	}
	
	gdv_os_deviceContext = GetDC(gdv_os_windowInstance);

	if(gdv_os_deviceContext == 0)
	{
		return(GDV_ERROR);
	}
	
	if(fullscreen == 1)
	{
		SetWindowPos(gdv_os_windowInstance, HWND_TOP, 0, 0, GetDeviceCaps( gdv_os_deviceContext, HORZRES), GetDeviceCaps( gdv_os_deviceContext, VERTRES), 0);
	}

	ShowWindow(gdv_os_windowInstance, SW_SHOWNORMAL);
	UpdateWindow(gdv_os_windowInstance);


	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	int pixelFormat;
	memset(&pixelFormatDescriptor, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pixelFormatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 8;
	pixelFormatDescriptor.cDepthBits = 16;
	pixelFormatDescriptor.cAccumBits = 32;
	pixelFormatDescriptor.cAccumRedBits = 8;
	pixelFormatDescriptor.cAccumGreenBits = 8;
	pixelFormatDescriptor.cAccumBlueBits = 8;
	pixelFormatDescriptor.cAccumAlphaBits = 8;
	pixelFormatDescriptor.cStencilBits = 8;
	pixelFormatDescriptor.dwLayerMask = PFD_MAIN_PLANE;

	pixelFormat = ChoosePixelFormat(gdv_os_deviceContext, &pixelFormatDescriptor);

	if(pixelFormat == 0)
	{
		return(GDV_ERROR);
	}


	if(SetPixelFormat(gdv_os_deviceContext, pixelFormat, &pixelFormatDescriptor) == FALSE)
	{
		return(GDV_ERROR);
	}

	if(gdv_os_renderingContext == NULL)
	{
		gdv_os_renderingContext = wglCreateContext(gdv_os_deviceContext);
	
		if(gdv_os_renderingContext == 0)
		{
			return(GDV_ERROR);
		}
	}

	if(wglMakeCurrent(gdv_os_deviceContext, gdv_os_renderingContext) == FALSE)
	{
		return(GDV_ERROR);
	}

	return(GDV_OK);
}

int gdv_os_closeWindow()
{
	int rc = GDV_OK;

	if(gdv_os_renderingContext != NULL)
	{
        if(wglMakeCurrent(NULL,NULL) == FALSE)
		{
			rc = GDV_ERROR;
		}

		/*if(wglDeleteContext(gdv_os_renderingContext) == FALSE)
		{
			//erc = GetLastError()
			//FormatMessage(erc ...)
			
			rc = GDV_ERROR;
		}

		gdv_os_renderingContext = NULL;*/
	}

	if((gdv_os_deviceContext != NULL) && (gdv_os_windowInstance != NULL))
	{
		if(ReleaseDC(gdv_os_windowInstance, gdv_os_deviceContext) == 0)
		{
			rc = GDV_ERROR;
		}
		
		gdv_os_deviceContext = NULL;
	}

	if(gdv_os_windowInstance != NULL)
	{
        if(DestroyWindow(gdv_os_windowInstance) == 0)
		{
			//erc = GetLastError()
			//FormatMessage(erc ...)
			
			rc = GDV_ERROR;
		}
		
		gdv_os_windowInstance = NULL;
	}

	return(rc);
}

void gdv_os_registerMainloopFunction(int (*mainloopFunction)(void))
{
	gdv_os_mainloopFunction = mainloopFunction;
}

void gdv_os_registerWindowEventFunction(void (*windowEventFunction)(GDV_BaseEvent* event))
{
	gdv_os_windowEventFunction = windowEventFunction;
}

unsigned __stdcall gdv_os_timeoutFunction(void* pArguments)
{
	Sleep(GDV_OS_TIMEOUT);
    
	printf("ERROR: The mainloop took to long to execute, possible hang situation.\n");
	fflush(stdout);

	exit(0);

    return 0;
}

int gdv_os_runMainloop()
{
	char stop = 0;
	MSG message;
	BOOL rc;

	static double mainLastTime = 0;
	mainLastTime = gdv_os_getCurrentTime();

	if(gdv_os_mainloopFunction == NULL)
	{
		return(GDV_ERROR);
	}

	do
	{
		//while(((gdv_os_getCurrentTime() - mainLastTime) < 0.05) && (PeekMessage(&message, NULL, 0, 0, PM_NOREMOVE) == TRUE))
		while(PeekMessage(&message, NULL, 0, 0, PM_NOREMOVE) == TRUE)
		{
			if((rc = GetMessage(&message, NULL, 0, 0)) != 0) 
			{ 
				if(rc == -1)
				{
					//erc = GetLastError()
					//FormatMessage(erc ...)
				}
				else
				{
					//TranslateMessage(&message); 
					DispatchMessage(&message);
				}
			}
			else
			{ 
				// WM_QUIT message was posted
				
				stop = 1;
			}
		}

		if(stop != 1)
		{
			unsigned threadID;
			HANDLE hThread = NULL;
			hThread = (HANDLE)_beginthreadex(NULL, 0, &gdv_os_timeoutFunction, NULL, 0, &threadID);

			stop = (*gdv_os_mainloopFunction)();
			
			glFlush();
			glFinish();
			SwapBuffers(gdv_os_deviceContext);
			
			if(hThread != NULL)
			{
				TerminateThread(hThread, 0);
			}

		}
	
		//mainLastTime = gdv_os_getCurrentTime();
	}
	while(stop != 1);

	return(GDV_OK);
}

void gdv_os_getWindowSize(float* width, float* height)
{
	*width = gdv_os_width;
	*height = gdv_os_height;
}

LONG WINAPI gdv_os_messageDispatchProcedure(HWND hWindow, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	LONG lRet = 1;
	GDV_EventType eventType = -1;
	int wheelDelta = 0;

	switch(uMessage)
	{
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				BeginPaint(hWindow, &ps);
				EndPaint(hWindow, &ps);
				break;
			}

		case WM_SIZE:
			{
				RECT rect;
				
				GetClientRect(hWindow, &rect);
				
				gdv_os_width = rect.right;
				gdv_os_height = rect.bottom;
				
				GDV_Event_Window_Size ws = {GDV_EVENT_WINDOW_SIZE, rect.right, rect.bottom};
				
				if(gdv_os_windowEventFunction != NULL)
				{
					gdv_os_windowEventFunction((GDV_BaseEvent*)&ws);
				}
				break;
			}

		case WM_CLOSE:
			gdv_os_closeWindow();
			PostQuitMessage(0);
			//TODO: Event close window / exit application
			break;

		case WM_DESTROY:
			//TODO: Event exit application
			break;

		case WM_LBUTTONDBLCLK:
			if(eventType == -1) eventType = GDV_EVENT_MOUSE_DOUBLE_CLICK_LEFT_BUTTON;
		case WM_LBUTTONDOWN:
			if(eventType == -1) eventType = GDV_EVENT_MOUSE_DOWN_LEFT_BUTTON;
		case WM_LBUTTONUP:
			if(eventType == -1) eventType = GDV_EVENT_MOUSE_UP_LEFT_BUTTON;
		case WM_MBUTTONDBLCLK:
			if(eventType == -1) eventType = GDV_EVENT_MOUSE_DOUBLE_CLICK_MIDDLE_BUTTON;
		case WM_MBUTTONDOWN:
			if(eventType == -1) eventType = GDV_EVENT_MOUSE_DOWN_MIDDLE_BUTTON;
		case WM_MBUTTONUP:
			if(eventType == -1) eventType = GDV_EVENT_MOUSE_UP_MIDDLE_BUTTON;
		case WM_RBUTTONDBLCLK:
			if(eventType == -1) eventType = GDV_EVENT_MOUSE_DOUBLE_CLICK_RIGHT_BUTTON;
		case WM_RBUTTONDOWN:
			if(eventType == -1) eventType = GDV_EVENT_MOUSE_DOWN_RIGHT_BUTTON;
		case WM_RBUTTONUP:
			if(eventType == -1) eventType = GDV_EVENT_MOUSE_UP_RIGHT_BUTTON;
		case WM_MOUSEWHEEL:
			if(eventType == -1)
			{
				eventType = GDV_EVENT_MOUSE_WHEEL;
				wheelDelta = HIWORD(wParam);
			}
		case WM_MOUSEMOVE:
			if(eventType == -1) eventType = GDV_EVENT_MOUSE_MOVE;

			if(gdv_os_mouseEventFunction != NULL)
			{
				GDV_Event_Mouse me = {eventType, LOWORD(lParam), HIWORD(lParam), wheelDelta};
				
				gdv_os_mouseEventFunction((GDV_BaseEvent*)&me);
			}
			break;

		case WM_SYSKEYUP:
			if(eventType == -1) eventType = GDV_EVENT_KEY_UP;
		case WM_SYSKEYDOWN:
			if(eventType == -1) eventType = GDV_EVENT_KEY_DOWN;
		case WM_KEYUP:
			if(eventType == -1) eventType = GDV_EVENT_KEY_UP;
		case WM_KEYDOWN:
			if(eventType == -1) eventType = GDV_EVENT_KEY_DOWN;
			
			if(gdv_os_keyEventFunction != NULL)
			{
				WORD receiveBuffer[3] = {0,0,0};
                unsigned int returnChar = 0;
				int tmpRC = 0;
                BYTE keyStates[256];
				memset(keyStates, 0, sizeof(BYTE) * 256);
                keyStates[VK_SHIFT] = GetKeyState(VK_SHIFT);
                keyStates[VK_CONTROL] = GetKeyState(VK_CONTROL);
                keyStates[VK_MENU] = GetKeyState(VK_MENU);

				tmpRC = ToAscii(wParam, lParam & 0xFF0000, (PBYTE)&keyStates, (LPWORD)&receiveBuffer, 0);
				
				if(tmpRC == 0)
				{
					returnChar = (int)wParam;
					
					switch(wParam)
					{
						case VK_F1:
							returnChar = GDV_KEY_F1;
							break;

						case VK_F2:
							returnChar = GDV_KEY_F2;
							break;

						case VK_F3:
							returnChar = GDV_KEY_F3;
							break;

						case VK_F4:
							returnChar = GDV_KEY_F4;
							break;

						case VK_F5:
							returnChar = GDV_KEY_F5;
							break;

						case VK_F6:
							returnChar = GDV_KEY_F6;
							break;

						case VK_F7:
							returnChar = GDV_KEY_F7;
							break;
						
						case VK_F8:
							returnChar = GDV_KEY_F8;
							break;

						case VK_F9:
							returnChar = GDV_KEY_F9;
							break;

						case VK_F10:
							returnChar = GDV_KEY_F10;
							break;

						case VK_F11:
							returnChar = GDV_KEY_F11;
							break;

						case VK_F12:
							returnChar = GDV_KEY_F12;
							break;

						case VK_PRIOR:
							returnChar = GDV_KEY_PAGE_UP;
							break;
						
						case VK_NEXT:
							returnChar = GDV_KEY_PAGE_DOWN;
							break;
							
						case VK_SHIFT:
							returnChar = GDV_KEY_SHIFT;
							break;
							
						case VK_CONTROL:
							if(HIWORD(lParam) & 0x100)
							{
								returnChar = GDV_KEY_RIGHT_CONTROL;
							}
							else
							{
								returnChar = GDV_KEY_LEFT_CONTROL;
							}
							
							break;
							
						case VK_MENU:
							returnChar = GDV_KEY_ALT;
							break;
						
						case VK_CLEAR:
							returnChar = GDV_KEY_CLEAR;
							break;

						case VK_PAUSE:
							returnChar = GDV_KEY_PAUSE;
							break;

						case VK_CAPITAL:
							returnChar = GDV_KEY_CAPS_LOCK;
							break;

						case VK_END:
							returnChar = GDV_KEY_END;
							break;

						case VK_HOME:
							returnChar = GDV_KEY_HOME;
							break;

						case VK_LEFT:
							returnChar = GDV_KEY_LEFT;
							break;

						case VK_UP:
							returnChar = GDV_KEY_UP;
							break;

						case VK_RIGHT:
							returnChar = GDV_KEY_RIGHT;
							break;

						case VK_DOWN:
							returnChar = GDV_KEY_DOWN;
							break;

						case VK_SNAPSHOT:
							returnChar = GDV_KEY_PRINT_SCREEN;
							break;

						case VK_INSERT:
							returnChar = GDV_KEY_INSERT;
							break;

						case VK_NUMLOCK:
							returnChar = GDV_KEY_NUM_LOCK;
							break;

						case VK_SCROLL:
							returnChar = GDV_KEY_SCROLL;
							break;
					}
				}
				else
				{
					returnChar = (int)receiveBuffer[0];
					
					if((returnChar >= 'A') && (returnChar <= 'Z'))
					{
						returnChar = returnChar - 'A' + 'a';
					}
				}

				if(returnChar == GDV_KEY_ESCAPE)  // Handle Esc here, don't let the user pinch of the exit possibility
				{
					gdv_os_exit();
				}
				else
				{
					GDV_Event_Keyboard ke = {eventType, returnChar};
					gdv_os_keyEventFunction((GDV_BaseEvent*)&ke);
				}
			}
			break;
		
		default:
			lRet = DefWindowProc(hWindow, uMessage, wParam, lParam);
			break;
	}

	return lRet;
}

double gdv_os_getCurrentTime()
{
	if(gdv_os_isPaused == 0)
	{
		LARGE_INTEGER curTime;
				
		if(gdv_os_timeWasInitialised == 0)
		{
			QueryPerformanceFrequency(&gdv_os_timeBaseFrequency);
			gdv_os_timeWasInitialised = 1;
		}
	
		QueryPerformanceCounter(&curTime);
			
		gdv_os_currentTime = (double)curTime.QuadPart / gdv_os_timeBaseFrequency.QuadPart;	
	}

	return(gdv_os_currentTime);
}

void gdv_os_toggleIsPause()
{
	if(gdv_os_isPaused == 0)
	{
		gdv_os_isPaused = 1;
	}
	else
	{
		gdv_os_isPaused = 0;
	}
}

void gdv_os_setWindowTitle(char* title)
{
	if(gdv_os_windowInstance != NULL)
	{
		SetWindowText(gdv_os_windowInstance, title);
	}
}

void gdv_os_showCursor(int visible)
{
	if(visible == 1)
	{
		ShowCursor(TRUE);
	}
	else
	{
		ShowCursor(FALSE);
	}
}

void gdv_os_registerMouseEventFunction(void (*mouseEventFunction)(GDV_BaseEvent* event))
{
	gdv_os_mouseEventFunction = mouseEventFunction;
}

void gdv_os_registerKeyEventFunction(void (*keyEventFunction)(GDV_BaseEvent* event))
{
	gdv_os_keyEventFunction = keyEventFunction;
}

void gdv_os_exit()
{
	PostQuitMessage(0);
}

int gdv_os_setMousePosition(long x, long y, int absoluteRelativeFlag, long* resultX, long* resultY)
{
	/*INPUT mouseInput;
	
	mouseInput.type = INPUT_MOUSE;
	mouseInput.mi.dx = x;
	mouseInput.mi.dy = y;
    mouseInput.mi.dwFlags = MOUSEEVENTF_MOVE;

	SendInput(1, &mouseInput, sizeof(mouseInput));*/

    if(absoluteRelativeFlag == 0)
	{
        if((gdv_os_windowInstance == NULL) || (gdv_os_deviceContext == NULL))
		{
			return(GDV_ERROR);
		}

		WINDOWINFO windwoInfo;
        
		GetWindowInfo(gdv_os_windowInstance, &windwoInfo);
		
		long rcx = ((long)windwoInfo.rcClient.left + (long)x) * (long)65535 / (long)GetDeviceCaps(gdv_os_deviceContext, HORZRES);
		long rcy = ((long)windwoInfo.rcClient.top + (long)y) * (long)65535 / (long)GetDeviceCaps(gdv_os_deviceContext, VERTRES);
		
		if(resultX != NULL) *resultX = (long)rcx * (long)GetDeviceCaps(gdv_os_deviceContext, HORZRES) / (long)65535 - (long)windwoInfo.rcClient.left;
		if(resultY != NULL) *resultY = (long)rcy * (long)GetDeviceCaps(gdv_os_deviceContext, VERTRES) / (long)65535 - (long)windwoInfo.rcClient.top;
		
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, rcx, rcy, 0, 0);
	}
	else
	{
		mouse_event(MOUSEEVENTF_MOVE, x, y, 0, 0);
		
		if(resultX != NULL) *resultX = x;
		if(resultY != NULL) *resultY = y;
	}

	return(GDV_OK);
}

int gdv_os_translateKeysToCommenTable(int key)
{
	int transKey = -1;

	switch(key)
	{
		case 1:
			break;
	}

	return(transKey);
}

