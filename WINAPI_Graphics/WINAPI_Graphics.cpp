#include "stdafx.h"
#include "WndClass.h"
#include "WINAPI_Graphics.h"

enum Direction { TOP, RIGHT, BOTTOM, LEFT };

bool CheckCoords(int, int, RECT);
void DoMove(HWND hWnd, long *firstPoint, long *secondPoint, int step);
int GetStep(Direction direction);
void RefreshFigure(HWND hWnd, long *firstPoint, long *secondPoint, int step);
void MovingWithTimer(HWND hWnd, int inOneSide, Direction direction);
int GetStepCountToBorder(RECT usrWnd, Direction direction);
bool IsMovingInBorder(RECT usrWnd, Direction direction);
void Moving(HWND hWnd, RECT usrWnd, long *firstPoint, long *secondPoint, Direction direction);
void Drawing(HDC hDC, HBRUSH brush);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

const int RECT_HEIGHT = 100, RECT_WIDTH = 300;
const int R = 100, G = 200, B = 200;
const int KEY_DIST = 40;

const int TIMER_TIME = 10, TIMER_STEP = 1;

int _inOneSide;
Direction _direction;
int _tempStep;

RECT shape = { 0, 0, RECT_WIDTH, RECT_HEIGHT };

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
		return usrWnd.bottom - shape.bottom;
	case TOP:
		return shape.top - usrWnd.top;
	case LEFT:
		return shape.left - usrWnd.left;
	case RIGHT:
		return usrWnd.right - shape.right;
	}
}

bool IsMovingInBorder(RECT usrWnd, Direction direction)
{
	switch (direction)
	{
	case TOP:
		if (shape.top - KEY_DIST < usrWnd.top)
			return true;
		break;
	case RIGHT:
		if (shape.right + KEY_DIST > usrWnd.right)
			return true;
		break;
	case BOTTOM:
		if (shape.bottom + KEY_DIST > usrWnd.bottom)
			return true;
		break;
	case LEFT:
		if (shape.left - KEY_DIST < usrWnd.left)
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
	Rectangle(hDC, shape.left, shape.top, shape.right, shape.bottom);
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
	HBRUSH brush = CreateSolidBrush(RGB(R, G, B));

	static RECT usrWnd;

	switch (uMsg)
	{		
	case WM_PAINT:
	{
		GetClientRect(hWnd, &usrWnd);
		InvalidateRect(hWnd, 0, TRUE);
		hDC = BeginPaint(hWnd, &ps);
		Drawing(hDC, brush);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		lbutDown.x = LOWORD(lParam);
		lbutDown.y = HIWORD(lParam);
		mouseDiffer.x = LOWORD(lParam) - shape.left;
		mouseDiffer.y = HIWORD(lParam) - shape.top;
		if (CheckCoords(lbutDown.x, lbutDown.y, shape))
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
			shape.left = LOWORD(lParam) - mouseDiffer.x;
			shape.top = HIWORD(lParam) - mouseDiffer.y;
			shape.right = shape.left + RECT_WIDTH;
			shape.bottom = shape.top + RECT_HEIGHT;
			InvalidateRect(hWnd, &shape, TRUE);
		}
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_DOWN:
		{
			Moving(hWnd, usrWnd, &shape.top, &shape.bottom, BOTTOM);
			break;
		}
		case VK_UP:
		{
			Moving(hWnd, usrWnd, &shape.top, &shape.bottom, TOP);
			break;
		}
		case VK_LEFT:
		{
			Moving(hWnd, usrWnd, &shape.left, &shape.right, LEFT);
			break;
		}
		case VK_RIGHT:
		{
			Moving(hWnd, usrWnd, &shape.left, &shape.right, RIGHT);
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
			DoMove(hWnd, &shape.left, &shape.right, -TIMER_STEP);
			break;
		}
		case TOP:
		{
			DoMove(hWnd, &shape.top, &shape.bottom, -TIMER_STEP);
			break;
		}
		case RIGHT:
		{
			DoMove(hWnd, &shape.left, &shape.right, TIMER_STEP);
			break;
		}
		case BOTTOM:
		{
			DoMove(hWnd, &shape.top, &shape.bottom, TIMER_STEP);
			break;
		}
		}
		_tempStep--;
		_inOneSide--;
		if (_tempStep == 0)
			KillTimer(hWnd, 1);
		break;
	}
	case WM_DESTROY:
	{
		DeleteObject(brush);
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
