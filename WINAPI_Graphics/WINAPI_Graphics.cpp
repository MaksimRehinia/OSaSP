#include "stdafx.h"
#include "WndClass.h"
#include "WINAPI_Graphics.h"

enum Direction { TOP, RIGHT, BOTTOM, LEFT };

bool CheckCoords(int x, int y, RECT shape);
void DoMove(HWND hWnd, long *firstPoint, long *secondPoint, int step);
int GetStep(Direction direction);
void RefreshFigure(HWND hWnd, long *firstPoint, long *secondPoint, int step);
void MovingWithTimer(HWND hWnd, int inOneSide, Direction direction);
int GetStepCountToBorder(RECT usrWnd, Direction direction);
bool IsMovingInBorder(RECT usrWnd, Direction direction);
void Moving(HWND hWnd, RECT usrWnd, long *firstPoint, long *secondPoint, Direction direction);
void Drawing(HDC hDC, HBRUSH brush);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

const int R = 100, G = 200, B = 200;
const int KEY_DIST = 40;
const int PICTURE_WIDTH = 220, PICTURE_HEIGHT = 216;
const int TIMER_TIME = 10, TIMER_STEP = 1;

int _inOneSide;
Direction _direction;
int _tempStep;

RECT picture;

bool CheckCoords(int x, int y, RECT shape)
{
	return (x >= shape.left && x <= shape.right &&
		    y >= shape.top  && y <= shape.bottom);
}

void DoMove(HWND hWnd, long *firstPoint, long *secondPoint, int step)
{
	if (_inOneSide > 0)
	{
		RefreshFigure(hWnd, firstPoint, secondPoint, step);
	}
	else
	{
		RefreshFigure(hWnd, firstPoint, secondPoint, -step);
	}
}

int GetStep(Direction direction)
{
	switch (direction)
	{
	case LEFT:
		return -KEY_DIST;
	case TOP:
		return -KEY_DIST;
	case BOTTOM:
		return KEY_DIST;
	case RIGHT:
		return KEY_DIST;
	}
}

void RefreshFigure(HWND hWnd, long *firstPoint, long *secondPoint, int step)
{
	*firstPoint += step;
	*secondPoint += step;
	InvalidateRect(hWnd, 0, TRUE);
}

void MovingWithTimer(HWND hWnd, int inOneSide, Direction direction)
{
	_direction = direction;
	_inOneSide = inOneSide;
	_tempStep = KEY_DIST;
	SetTimer(hWnd, 1, TIMER_TIME, NULL);
}

int GetStepCountToBorder(RECT usrWnd, Direction direction)
{
	switch (direction)
	{
	case BOTTOM:
		return usrWnd.bottom - picture.bottom;
	case TOP:
		return picture.top - usrWnd.top;
	case LEFT:
		return picture.left - usrWnd.left;
	case RIGHT:
		return usrWnd.right - picture.right;
	}
}

bool IsMovingInBorder(RECT usrWnd, Direction direction)
{
	switch (direction)
	{
	case TOP:
		if (picture.top - KEY_DIST < usrWnd.top)
			return true;
		break;
	case RIGHT:
		if (picture.right + KEY_DIST > usrWnd.right)
			return true;
		break;
	case BOTTOM:
		if (picture.bottom + KEY_DIST > usrWnd.bottom)
			return true;
		break;
	case LEFT:
		if (picture.left - KEY_DIST < usrWnd.left)
			return true;
	}
	return false;
}

void Moving(HWND hWnd, RECT usrWnd, long *firstPoint, long *secondPoint, Direction direction)
{	
	if (IsMovingInBorder(usrWnd, direction))
	{
		int stepCountToBorder = GetStepCountToBorder(usrWnd, direction);
		MovingWithTimer(hWnd, stepCountToBorder, direction);
	}
	else
		RefreshFigure(hWnd, firstPoint, secondPoint, GetStep(direction));
}

void Drawing(HDC hDC, HBRUSH brush)
{
	SelectObject(hDC, brush);
	Rectangle(hDC, picture.left, picture.top, picture.right, picture.bottom);
}

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
	static POINT lbutDown, mouseDiffer;
	static bool redrawRect = false;
	static HBRUSH brush = NULL;
	static HBITMAP hBitmap = NULL;

	static HDC hdcMem = NULL;

	static RECT usrWnd;
	static HGDIOBJ oldBitmap = NULL;
	switch (uMsg)
	{	
	case WM_CREATE:
	{
		brush = CreateSolidBrush(RGB(R, G, B));
		hBitmap = (HBITMAP)LoadImage(NULL, L"ball.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hdcMem = CreateCompatibleDC(GetDC(0));
		oldBitmap = SelectObject(hdcMem, hBitmap);
		picture.left = picture.top = 0;
		picture.right = PICTURE_WIDTH;
		picture.bottom = PICTURE_HEIGHT;
		break;
	}
	case WM_PAINT:
	{						
		InvalidateRect(hWnd, 0, TRUE);
		hDC = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &usrWnd);
		//Drawing(hDC, brush);						
		BitBlt(hDC, picture.left, picture.top, PICTURE_WIDTH, PICTURE_HEIGHT, hdcMem, 0, 0, SRCCOPY);		
		EndPaint(hWnd, &ps);	
		break;
	}
	case WM_LBUTTONDOWN:
	{
		lbutDown.x = LOWORD(lParam);
		lbutDown.y = HIWORD(lParam);
		mouseDiffer.x = LOWORD(lParam) - picture.left;
		mouseDiffer.y = HIWORD(lParam) - picture.top;
		if (CheckCoords(lbutDown.x, lbutDown.y, picture))
			redrawRect = true;
		break;
	}
	case WM_LBUTTONUP:
	{
		redrawRect = false;
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (redrawRect)
		{
			picture.left = LOWORD(lParam) - mouseDiffer.x;
			picture.top = HIWORD(lParam) - mouseDiffer.y;
			picture.right = picture.left + PICTURE_WIDTH;
			picture.bottom = picture.top + PICTURE_HEIGHT;
			InvalidateRect(hWnd, &picture, TRUE);
		}
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_DOWN:
		{
			Moving(hWnd, usrWnd, &picture.top, &picture.bottom, BOTTOM);
			break;
		}
		case VK_UP:
		{
			Moving(hWnd, usrWnd, &picture.top, &picture.bottom, TOP);
			break;
		}
		case VK_LEFT:
		{
			Moving(hWnd, usrWnd, &picture.left, &picture.right, LEFT);
			break;
		}
		case VK_RIGHT:
		{
			Moving(hWnd, usrWnd, &picture.left, &picture.right, RIGHT);
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_TIMER:
	{
		switch (_direction)
		{
		case LEFT:
		{
			DoMove(hWnd, &picture.left, &picture.right, -TIMER_STEP);
			break;
		}
		case TOP:
		{
			DoMove(hWnd, &picture.top, &picture.bottom, -TIMER_STEP);
			break;
		}
		case RIGHT:
		{
			DoMove(hWnd, &picture.left, &picture.right, TIMER_STEP);
			break;
		}
		case BOTTOM:
		{
			DoMove(hWnd, &picture.top, &picture.bottom, TIMER_STEP);
			break;
		}
		}
		_tempStep--;
		_inOneSide--;
		if (_tempStep == 0)
			KillTimer(hWnd, 1);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		int keyState = GET_KEYSTATE_WPARAM(wParam);
		int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (keyState == MK_SHIFT)
		{
			if (delta > 0)
			{
				Moving(hWnd, usrWnd, &picture.left, &picture.right, RIGHT);
			}
			else
			{
				Moving(hWnd, usrWnd, &picture.left, &picture.right, LEFT);
			}
		}
		else
		{
			if (delta > 0)
			{
				Moving(hWnd, usrWnd, &picture.top, &picture.bottom, TOP);
			}
			else
			{
				Moving(hWnd, usrWnd, &picture.top, &picture.bottom, BOTTOM);
			}
		}
		break;
	}
	case WM_DESTROY:
	{
		SelectObject(hdcMem, oldBitmap);
		DeleteDC(hdcMem);
		DeleteObject(brush);
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
