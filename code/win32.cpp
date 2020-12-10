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
	//pixels are always 32-bits wide, Memory Order BB GG RR XX
	BITMAPINFO Info;
	void* Memory;
	int Width;
	int Height;
	int Pitch;
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

global_variable bool GlobalRunning = 0;
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

			*Pixel = ( ( Green << 8 ) | Blue );
			++Pixel;
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
	int BytesPerPixel = 4;

	Buffer->Width = Width;
	Buffer->Height = Height;
	Buffer->Info.bmiHeader.biSize = sizeof( Buffer->Info.bmiHeader );
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height;	//when the biHeight field is negative, this is the clue to Windows to treat this bitmap as top-down instead of bottom-up. i.e. the first three bytes of the image are the colour for the top left pixel in the bitmap, not the bottom left
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;
	Buffer->Info.bmiHeader.biSizeImage = 0;
	Buffer->Info.bmiHeader.biXPelsPerMeter = 0;
	Buffer->Info.bmiHeader.biYPelsPerMeter = 0;
	Buffer->Info.bmiHeader.biClrUsed = 0;
	Buffer->Info.bmiHeader.biClrImportant = 0;

	int BitmapMemorySize = ( Buffer->Width * Buffer->Height ) * BytesPerPixel;
	Buffer->Memory = VirtualAlloc( 0 , BitmapMemorySize , MEM_COMMIT , PAGE_READWRITE );

	//to do clear window to black
	Buffer->Pitch = Width * BytesPerPixel;
}

internal void Win32DisplayBufferInWindow(
	HDC DeviceContext ,
	int WindowWidth ,
	int WindowHeight ,
	win32_offscreen_buffer Buffer
)
{
	//to do aspect ratio correction
	//to do play with stretch modes
	StretchDIBits(
		DeviceContext ,
		0 ,				//destination
		0 ,				//destination
		WindowWidth ,	//destination
		WindowHeight ,	//destination
		0 ,				//source
		0 ,				//source
		Buffer.Width ,	//source	
		Buffer.Height ,	//source
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
		case WM_CLOSE:
		{
			GlobalRunning = false;	//to do handle with message to the user
		}break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA( "WM_ACTIVATEAPP\n" );
		}break;

		case WM_DESTROY:
		{
			GlobalRunning = false;	//to do handle this as an error and recreate window
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
			Win32DisplayBufferInWindow( DeviceContext , Dimension.Width , Dimension.Height , GlobalBackbuffer );
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
	//all of these codes from here ...
	WNDCLASS WindowClass = {};	//initialise a window and clear to zero

	//win32_window_dimension Dimension = Win32GetWindowDimension( Window );
	Win32ResizeDIBSection( &GlobalBackbuffer , 1280 , 720 );

	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WindowClass.lpfnWndProc = Win32MainWindowCallBack;
	WindowClass.hInstance = Instance;
	//WindowClass.hIcon = ;	//to do later
	WindowClass.lpszClassName = "GameWindowClass";	//name for window class so we can call this to create a window

	//... to here goes into the stack

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
			HDC DeviceContext = GetDC( Window ); //since we specified CS_OWNDC, we can just get one device context and use it forever because we are not sharing it with anyone.
			int XOffset = 0;
			int YOffset = 0;
			GlobalRunning = true;

			while ( GlobalRunning )
			{
				MSG Message;
				while ( PeekMessage( &Message , 0 , 0 , 0 , PM_REMOVE ) )
				{
					if ( Message.message == WM_QUIT )
					{
						GlobalRunning = false;
					}
					TranslateMessage( &Message );
					DispatchMessage( &Message );
				}
				RenderWeirdGradient( GlobalBackbuffer , XOffset , YOffset );
				win32_window_dimension Dimension = Win32GetWindowDimension( Window );
				Win32DisplayBufferInWindow( DeviceContext , Dimension.Width , Dimension.Height , GlobalBackbuffer );
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