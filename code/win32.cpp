/* ==================================================================== *
 * File:	#							*
 * Date:	#							*
 * Revision:	#							*
 * Creator:	Amir Yunus						*
 * Notice: (C) Copyright 2020 by Amir Yunus. All Rights Reserved.	*
 * ==================================================================== */

#include <windows.h> //include windows header files

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) //WinMain entry point
{
	MessageBoxA(0, "This is cpp_game", "cpp_game", MB_OK | MB_ICONINFORMATION); //Prompt message box to user
	return(0);
}
