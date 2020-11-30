/* ================================================================ *
 * File:	#														*
 * Date:	#														*
 * Revision:	#													*
 * Creator:	Amir Yunus												*
 * Notice: (C) Copyright 2020 by Amir Yunus. All Rights Reserved.	*
 * ================================================================ */

#include <windows.h> //include windows header files

LRESULT CALLBACK MainWindowCallBack(HWND Window,
									UINT Message,
									WPARAM WParam,
									LPARAM LParam) //function that processes messages sent to a window
{
	LRESULT Result = 0;

	switch (Message)
	{
		case WM_SIZE:
		{
			OutputDebugStringA("WM_SIZE\n");
		}break;

		case WM_DESTROY:
		{
			OutputDebugStringA("WM_DESTROY\n");
		}break;

		case WM_CLOSE:
		{
			OutputDebugStringA("WM_CLOSE\n");
		}break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		}break;

		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;
			static DWORD Operation = WHITENESS;
			PatBlt(DeviceContext, X, Y, Width, Height, Operation);
			if (Operation == WHITENESS)
			{
				Operation = BLACKNESS;
			}
			else
			{
				Operation = WHITENESS;
			}
			EndPaint(Window, &Paint);
		}break;

		default:
		{
			//OutputDebugStringA("default\n");
			Result = DefWindowProc(Window, Message, WParam, LParam);
		}break;
	}
	return(Result);
}

int WINAPI wWinMain(HINSTANCE Instance,
					HINSTANCE PrevInstance,
					PWSTR CommandLine,
					int ShowCode) //WinMain entry point
{
	//MessageBoxA(0, "This is cpp_game", "cpp_game", MB_OK | MB_ICONINFORMATION); //Prompt message box to user
	WNDCLASS WindowClass = {}; //initialise a window and clear to zero
	
	WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW; //initialise flags for the device context || to check if redraw / owndc still relevant
	WindowClass.lpfnWndProc = MainWindowCallBack;
	WindowClass.hInstance = Instance;
	//WindowClass.hIcon = ; //to do later
	WindowClass.lpszClassName = "GameWindowClass"; //name for window class so we can call this to create a window

	if (RegisterClass(&WindowClass))
	{
		HWND WindowHandle = CreateWindowEx(
											0,
											WindowClass.lpszClassName,
											"cpp_game",
											WS_OVERLAPPEDWINDOW|WS_VISIBLE,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											0,
											0,
											Instance,
											0);
		if(WindowHandle != NULL)
		{
			for(;;)
			{
				MSG Message;
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if(MessageResult > 0)
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				else
				{
					break;
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
	return(0);
}