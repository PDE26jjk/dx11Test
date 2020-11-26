#include "window.h"
#include "App.h"
#include <sstream>
//LRESULT CALLBACK WinProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam){
//
//	switch (msg) {
//	case WM_CLOSE:
//		PostQuitMessage(69);
//		break;
//	case WM_KEYDOWN:
//		if (wParam == 'F')
//			SetWindowText(hWnd, "eeeeeeeee");
//		break;
//	case WM_KEYUP:
//		if (wParam == 'F')
//			SetWindowText(hWnd, "aaaaaaaaaaa");
//		break;
//	case WM_CHAR: {
//		static std::string title;
//		title.push_back((char)wParam);
//		SetWindowText(hWnd, title.c_str());
//		}
//		break;  
//	case WM_LBUTTONDOWN: 
//		const POINTS pt = MAKEPOINTS(lParam);
//		std::ostringstream oss;
//		oss << "(" << pt.x << "," << pt.y << ")";
//		SetWindowText(hWnd, oss.str().c_str());
//		break;
//	}
//
//	return DefWindowProc(hWnd, msg, wParam, lParam);
//}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd) {
		
	return App{}.Go();
}