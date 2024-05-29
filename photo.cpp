#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

enum ImageState
{
	STRIPES_IMAGE,
	COLOR_IMAGE
};

ImageState currentImageState = STRIPES_IMAGE;
HWND hwnd;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static ULONG_PTR gdiplusToken;
	switch (uMsg)
	{
	case WM_CREATE:
		{
			GdiplusStartupInput gdiplusStartupInput;
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
			break;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			
			RECT clientRect;
			GetClientRect(hwnd, &clientRect);
			int width = clientRect.right - clientRect.left;
			int height = clientRect.bottom - clientRect.top;
			
			Graphics graphics(hdc);
			
			if (currentImageState == STRIPES_IMAGE)
			{
				Bitmap bitmap(L"stripes.bmp", FALSE); // 从 stripes.bmp 文件加载位图
				graphics.DrawImage(&bitmap, 0, 0, width, height);
			}
			else if (currentImageState == COLOR_IMAGE)
			{
				Bitmap bitmap(L"color_image.bmp", FALSE); // 从 color_image.bmp 文件加载位图
				graphics.DrawImage(&bitmap, 0, 0, width, height);
			}
			
			EndPaint(hwnd, &ps);
		}
		break;
	case WM_TIMER:
		KillTimer(hwnd, 1);  // 清除之前的定时器
		if (currentImageState == STRIPES_IMAGE)
		{
			currentImageState = COLOR_IMAGE;
			InvalidateRect(hwnd, NULL, TRUE);  // 重新绘制窗口以显示新图像
			SetTimer(hwnd, 1, 4000, NULL);  // 设置一个4秒的定时器
		}
		else if (currentImageState == COLOR_IMAGE)
		{
			DestroyWindow(hwnd); // 关闭窗口
		}
		break;
	case WM_DESTROY:
		GdiplusShutdown(gdiplusToken);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	
	return 0;
}

int main()
{
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = TEXT("MyWindowClass");
	RegisterClass(&wc);
	
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	
	hwnd = CreateWindowEx(
		0,
		TEXT("MyWindowClass"),
		TEXT("Image Display"),
		WS_POPUP | WS_VISIBLE,
		0, 0, screenWidth, screenHeight,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	
	if (hwnd == NULL)
	{
		return 0;
	}
	
	SetTimer(hwnd, 1, 2000, NULL);  // 设置一个2秒的定时器
	
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return 0;
}
