
/*
MIT License

Copyright (c) 2022 Gothic Multiplayer Team (pampi, skejt23, mecio)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "CGothicStarter.h"
#include <string>

//#define ROOT_PATH "D:\\Gothic\\System"
#define ROOT_PATH "E:\\Gothic IIpl\\System"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//Pozniej mozna dodac jakies okienko ala Launcher pampiego
	std::string GothicRootPath(ROOT_PATH);
	CGothicStarter starter(GothicRootPath);
	std::string GameName("GothicGame"); 
	std::string DllName("GMP.Dll");
	if(!starter.StartGothic("GothicGame", CGothicStarter::WINDOW_MODE|CGothicStarter::NO_MENU, DllName))
	{
		MessageBoxA(0, "L�L", "ERROR", MB_ICONERROR|MB_OK);
	}

	return 0;
}