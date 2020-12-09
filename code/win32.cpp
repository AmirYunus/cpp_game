/* ================================================================ *
 * File:	#														*
 * Date:	#														*
 * Revision:	#													*
 * Creator:	Amir Yunus												*
 * Notice: (C) Copyright 2020 by Amir Yunus. All Rights Reserved.	*
 * ================================================================ */

#include <windows.h>	//include windows header files
#include <stdint.h>	

#define internal static	//define static functions as internal
#define local_persist static	//define local static as local_persist
#define global_variable static	//define global static as global_variable

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

struct win32_offscreen_buffer
{
	BITMAPINFO Info;
	void* Memory;
	int Width;
	int Height;
	int Pitch;
	int BytesPerPixel;
};

struct win32_window_dimension
{
	int Width;
	int Height;
};

win32_window_dimension Win32GetWindowDimension( HWND Window )
{
	win32_window_dimension Result;

	RECT ClientRect;
	GetClientRect( Window , &ClientRect );
	Result.Width = ClientRect.right - ClientRect.left;
	Result.Height = ClientRect.bottom - ClientRect.top;

	return( Result );
};

global_variable bool Running = 0;
global_variable win32_offscreen_buffer GlobalBackbuffer;

internal void RenderWeirdGradient( win32_offscreen_buffer Buffer , int XOffset , int YOffset )
{
	uint8* Row = ( uint8* ) Buffer.Memory;
	for ( int Y = 0;
		Y < Buffer.Height;
		++Y )
	{
		uint32* Pixel = ( uint32* ) Row;
		for ( int X = 0;
			X < Buffer.Width;
			++X )
		{
			uint8 Blue = ( X + XOffset );
			uint8 Green = ( Y + YOffset );

			*Pixel++ = ( ( Green << 8 ) | Blue );
		}
		Row += Buffer.Pitch;
	}
}

internal void Win32ResizeDIBSection(
	win32_offscreen_buffer* Buffer ,
	int Width ,
	int Height
)	//DIB: Device Independent Bitmap
{
	//to do improve memory usage

	if ( Buffer->Memory )
	{
		VirtualFree( Buffer->Memory , 0 , MEM_RELEASE );
	}

	Buffer->Width = Width;
	Buffer->Height = Height;
	Buffer->BytesPerPixel = 4;
	Buffer->Info.bmiHeader.biSize = sizeof( Buffer->Info.bmiHeader );
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;
	Buffer->Info.bmiHeader.biSizeImage = 0;
	Buffer->Info.bmiHeader.biXPelsPerMeter = 0;
	Buffer->Info.bmiHeader.biYPelsPerMeter = 0;
	Buffer->Info.bmiHeader.biClrUsed = 0;
	Buffer->Info.bmiHeader.biClrImportant = 0;

	int BitmapMemorySize = ( Buffer->Width * Buffer->Height ) * Buffer->BytesPerPixel;
	Buffer->Memory = VirtualAlloc( 0 , BitmapMemorySize , MEM_COMMIT , PAGE_READWRITE );

	//to do clear window to black
	Buffer->Pitch = Width * Buffer->BytesPerPixel;
}

internal void Win32DisplayBufferInWindow(
	HDC DeviceContext ,
	int WindowWidth,
	int WindowHeight,
	win32_offscreen_buffer Buffer ,
	int	X ,
	int Y ,
	int Width ,
	int Height
)
{
	StretchDIBits(
		DeviceContext ,
		0 ,	//X,	
		0 ,	//Y,	
		Buffer.Width ,	//Width,	
		Buffer.Height ,	//Height,	
		0 ,	//X,	
		0 ,	//Y,		//temporary global variable; to change later
		WindowWidth ,	//Width,	
		WindowHeight ,	//Height,	
		Buffer.Memory ,
		&Buffer.Info ,
		DIB_RGB_COLORS , SRCCOPY
	);
}

LRESULT CALLBACK Win32MainWindowCallBack(
	HWND Window ,
	UINT Message ,
	WPARAM WParam ,
	LPARAM LParam
)	//function that processes messages sent to a window
{
	LRESULT Result = 0;

	switch ( Message )
	{
		case WM_SIZE:
		{
			win32_window_dimension Dimension = Win32GetWindowDimension( Window );
			Win32ResizeDIBSection( &GlobalBackbuffer , Dimension.Width , Dimension.Height );
		}break;

		case WM_CLOSE:
		{
			Running = false;	//to do handle with message to the user
		}break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA( "WM_ACTIVATEAPP\n" );
		}break;

		case WM_DESTROY:
		{
			Running = false;	//to do handle this as an error and recreate window
		}break;

		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint( Window , &Paint );
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;
			
			win32_window_dimension Dimension = Win32GetWindowDimension( Window );
			Win32DisplayBufferInWindow( DeviceContext , Window.Width, Window.Height , GlobalBackbuffer , X , Y , Width , Height );
			EndPaint( Window , &Paint );
		}break;

		default:
		{
			Result = DefWindowProc( Window , Message , WParam , LParam );
		}break;
	}
	return( Result );
}

int WINAPI wWinMain( HINSTANCE Instance ,
	HINSTANCE PrevInstance ,
	PWSTR CommandLine ,
	int ShowCode
) //WinMain entry point
{
	//MessageBoxA(0, "This is cpp_game", "cpp_game", MB_OK | MB_ICONINFORMATION); //Prompt message box to user
	WNDCLASS WindowClass = {};	//initialise a window and clear to zero

	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallBack;
	WindowClass.hInstance = Instance;
	//WindowClass.hIcon = ;	//to do later
	WindowClass.lpszClassName = "GameWindowClass";	//name for window class so we can call this to create a window

	if ( RegisterClass( &WindowClass ) )
	{
		HWND Window = CreateWindowExA(
			0 ,
			WindowClass.lpszClassName ,
			"cpp_game" ,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE ,
			CW_USEDEFAULT ,
			CW_USEDEFAULT ,
			CW_USEDEFAULT ,
			CW_USEDEFAULT ,
			0 ,
			0 ,
			Instance ,
			0
		);
		if ( Window != NULL )
		{
			int XOffset = 0;
			int YOffset = 0;
			Running = true;

			while ( Running )
			{
				MSG Message;
				while ( PeekMessage( &Message , 0 , 0 , 0 , PM_REMOVE ) )
				{
					if ( Message.message == WM_QUIT )
					{
						Running = false;
					}
					TranslateMessage( &Message );
					DispatchMessage( &Message );
				}
				RenderWeirdGradient( GlobalBackbuffer , XOffset , YOffset );
				HDC DeviceContext = GetDC( Window );

				win32_window_dimension Dimension = Win32GetWindowDimension( Window );
				Win32DisplayBufferInWindow( DeviceContext , ClientRect , GlobalBackbuffer, 0 , 0 , Dimension.Width , Dimension.Height );
				ReleaseDC( Window , DeviceContext );
				++XOffset;
				YOffset += 2;
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
	return( 0 );
}