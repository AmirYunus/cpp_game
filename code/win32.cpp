/* ================================================================ *
 * File:	#														*
 * Date:	#														*
 * Revision:	#													*
 * Creator:	Amir Yunus												*
 * Notice: (C) Copyright 2020 by Amir Yunus. All Rights Reserved.	*
 * ================================================================ */

#include <windows.h>	//include windows header files
#define internal static	//define static functions as internal
#define local_persist static	//define local static as local_persist
#define global_variable static	//define global static as global_variable

global_variable bool Running = 0;	//temporary global variable; to change later
global_variable BITMAPINFO BitmapInfo;	//temporary global variable; to change later
global_variable void* BitmapMemory;	//temporary global variable; to change later
global_variable HBITMAP BitmapHandle;	//temporary global variable; to change later
global_variable HDC BitmapDeviceContext;	//temporary global variable; to change later

internal void Win32ResizeDIBSection(int Width,	//to insert comment
									int Height	//to insert comment
)	//DIB: Device Independent Bitmap
{
	//to do improve memory usage
	if(BitmapHandle)	//to insert comment
	{
		DeleteObject(BitmapHandle);	//to insert comment
	}
	
	if(BitmapDeviceContext != 0)	//to insert comment
	{
		//to do check if need to recreate under edge cases
		HDC DeviceContext = CreateCompatibleDC(0);	//to insert comment
	}
	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);	//to insert comment
	BitmapInfo.bmiHeader.biWidth = Width;	//to insert comment
	BitmapInfo.bmiHeader.biHeight = Height;	//to insert comment
	BitmapInfo.bmiHeader.biPlanes = 1;	//to insert comment
	BitmapInfo.bmiHeader.biBitCount = 32;	//to insert comment
	BitmapInfo.bmiHeader.biCompression = BI_RGB;	//to insert comment
	BitmapInfo.bmiHeader.biSizeImage = 0;	//to insert comment
	BitmapInfo.bmiHeader.biXPelsPerMeter = 0;	//to insert comment
	BitmapInfo.bmiHeader.biYPelsPerMeter = 0;	//to insert comment
	BitmapInfo.bmiHeader.biClrUsed = 0;	//to insert comment
	BitmapInfo.bmiHeader.biClrImportant = 0;	//to insert comment

	//to do maybe we can allocate this ourselves instead on relying on Windows
	BitmapHandle = CreateDIBSection(BitmapDeviceContext,	//to insert comment
									&BitmapInfo,	//to insert comment
									DIB_RGB_COLORS,	//to insert comment
									&BitmapMemory,	//to insert comment
									0,	//to insert comment
									0	//to insert comment
	);	//to insert comment
}

internal void Win32UpdateWindow(HDC DeviceContext,	//to insert comment
								int	X,	//to insert comment
								int Y,	//to insert comment
								int Width,	//to insert comment
								int Height	//to insert comment
)	//to insert comment
{
	StretchDIBits(	DeviceContext,	//to insert comment
					X,	//to insert comment
					Y,	//to insert comment
					Width,	//to insert comment
					Height,	//to insert comment
					X,	//to insert comment
					Y,	//to insert comment
					Width,	//to insert comment
					Height,	//to insert comment
					BitmapMemory,	//to insert comment
					&BitmapInfo,	//to insert comment
					DIB_RGB_COLORS, SRCCOPY	//to insert comment
	);	//to insert comment
}

LRESULT CALLBACK Win32MainWindowCallBack(	HWND Window,	//to insert comment
											UINT Message,	//to insert comment
											WPARAM WParam,	//to insert comment
											LPARAM LParam	//to insert comment
)	//function that processes messages sent to a window
{
	LRESULT Result = 0;	//to insert comment

	switch (Message)	//to insert comment
	{
		case WM_SIZE:	//to insert comment
		{
			RECT ClientRect;	//to insert comment
			GetClientRect(Window, &ClientRect);	//to insert comment
			int Width = ClientRect.right - ClientRect.left;	//to insert comment
			int Height = ClientRect.bottom - ClientRect.top;	//to insert comment
			Win32ResizeDIBSection(Width, Height);	//to insert comment
			//OutputDebugStringA("WM_SIZE\n");
		}break;

		case WM_CLOSE:	//to insert comment
		{
			Running = false;	//to do handle with message to the user
			//OutputDebugStringA("WM_CLOSE\n");
		}break;

		case WM_ACTIVATEAPP:	//to insert comment
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");	//to insert comment
		}break;

		case WM_DESTROY:	//to insert comment
		{
			Running = false;	//to do handle this as an error and recreate window
			//OutputDebugStringA("WM_DESTROY\n");
		}break;

		case WM_PAINT:	//to insert comment
		{
			PAINTSTRUCT Paint;	//to insert comment
			HDC DeviceContext = BeginPaint(Window, &Paint);	//to insert comment
			int X = Paint.rcPaint.left;	//to insert comment
			int Y = Paint.rcPaint.top;	//to insert comment
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;	//to insert comment
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;	//to insert comment
			Win32UpdateWindow(DeviceContext, X, Y, Width, Height);	//to insert comment
			//local_persist DWORD Operation = WHITENESS;	//to insert comment
			//PatBlt(DeviceContext, X, Y, Width, Height, Operation);	//to insert comment
			//if (Operation == WHITENESS)	//to insert comment
			//{
			//	Operation = BLACKNESS;	//to insert comment
			//}
			//else 
			//{
			//	Operation = WHITENESS;	//to insert comment
			//}
			EndPaint(Window, &Paint);	//to insert comment
		}break;	//to insert comment

		default:	//to insert comment
		{
			//OutputDebugStringA("default\n");
			Result = DefWindowProc(Window, Message, WParam, LParam);	//to insert comment
		}break;	//to insert comment
	}
	return(Result);	//to insert comment
}

int WINAPI wWinMain(HINSTANCE Instance,	//to insert comment
					HINSTANCE PrevInstance,	//to insert comment
					PWSTR CommandLine,	//to insert comment
					int ShowCode	//to insert comment
) //WinMain entry point
{
	//MessageBoxA(0, "This is cpp_game", "cpp_game", MB_OK | MB_ICONINFORMATION); //Prompt message box to user
	WNDCLASS WindowClass = {};	//initialise a window and clear to zero
	
	WindowClass.lpfnWndProc = Win32MainWindowCallBack;	//to insert comment
	WindowClass.hInstance = Instance;	//to insert comment
	//WindowClass.hIcon = ;	//to do later
	WindowClass.lpszClassName = "GameWindowClass";	//name for window class so we can call this to create a window

	if (RegisterClass(&WindowClass))	//to insert comment
	{
		HWND WindowHandle = CreateWindowExA(0,	//to insert comment
											WindowClass.lpszClassName,	//to insert comment
											"cpp_game",	//to insert comment
											WS_OVERLAPPEDWINDOW|WS_VISIBLE,	//to insert comment
											CW_USEDEFAULT,	//to insert comment
											CW_USEDEFAULT,	//to insert comment
											CW_USEDEFAULT,	//to insert comment
											CW_USEDEFAULT,	//to insert comment
											0,	//to insert comment
											0,	//to insert comment
											Instance,	//to insert comment
											0	//to insert comment
		);	//to insert comment
		if(WindowHandle != NULL)	//to insert comment
		{
			Running = true;	//to insert comment
			while(Running)	//to insert comment
			{
				MSG Message;	//to insert comment
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);	//to insert comment
				if(MessageResult > 0)	//to insert comment
				{
					TranslateMessage(&Message);	//to insert comment
					DispatchMessage(&Message);	//to insert comment
				}
				else
				{
					break;	//to insert comment
				}
			}
		}
		else
		{
			//to do error handling
		}
	}
	else
	{
		//to do logging later
	}
	return(0);	//to insert comment
}