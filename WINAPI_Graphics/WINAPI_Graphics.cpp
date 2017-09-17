#include "stdafx.h"
#include "WndClass.h"
#include "WINAPI_Graphics.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CreateRect(HDC, RECT, HBRUSH);

const int RECT_HEIGHT = 100, RECT_WIDTH = 300;
const int R = 100, G = 200, B = 200;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int
	nCmdShow)
{
	MSG msg;
	WndClass mainWnd(L"A WINAPI_Graphics application", hInstance, nCmdShow, WndProc);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;	
	PAINTSTRUCT ps;
	static RECT rect = { 0, 0, RECT_WIDTH, RECT_HEIGHT };
	HBRUSH brush = (HBRUSH)CreateSolidBrush(RGB(R, G, B));

	switch (uMsg)
	{		
	case WM_PAINT:
	{		
		InvalidateRect(hWnd, 0, TRUE);
		hDC = BeginPaint(hWnd, &ps);		
		CreateRect(hDC, rect, brush);
		EndPaint(hWnd, &ps);
		break;
	}	
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

void CreateRect(HDC hDC, RECT rect, HBRUSH brush)
{
	Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
	FillRect(hDC, &rect, brush);
}
