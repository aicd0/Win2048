#include "framework.h"
#include "framework/automation.h"
#include "Win2048.h"
#include "paint.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM        MyRegisterClass(HINSTANCE hInstance);
BOOL        InitInstance(HINSTANCE, int);
LRESULT CALLBACK  WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK  About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPWSTR  lpCmdLine,
  _In_ int     nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, IDC_WIN2048, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  if (!InitInstance(hInstance, nCmdShow))
  {
    return FALSE;
  }

  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN2048));

  MSG msg;

  while (GetMessage(&msg, nullptr, 0, 0))
  {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN2048));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WIN2048);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}

///////////////////////////
#define FPS 30

PaintCore paintCore;
Automation2048 automation2048;
Win2048 &game2048 = automation2048.MainTable;
bool keyboardMode = true;
LARGE_INTEGER timer;
double timerFrequency;
__int64 lastRecord;
///////////////////////////

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  hInst = hInstance;
  HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
  if (!hWnd)
    return FALSE;

  QueryPerformanceFrequency(&timer);
  timerFrequency = (double)timer.QuadPart;
  QueryPerformanceCounter(&timer);
  lastRecord = timer.QuadPart;
  SetTimer(hWnd, 1, 1000 / FPS, NULL);
  SetTimer(hWnd, 2, 1000, NULL);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_KEYDOWN:
    switch (wParam)
    {
    case VK_SPACE:
      keyboardMode = !keyboardMode;
      break;
    case VK_UP:
      if (keyboardMode) game2048.up();
      break;
    case VK_DOWN:
      if (keyboardMode) game2048.down();
      break;
    case VK_RIGHT:
      if (keyboardMode) game2048.right();
      break;
    case VK_LEFT:
      if (keyboardMode) game2048.left();
      break;
    case VK_RETURN:
      game2048.newGame();
      break;
    }
    break;

  case WM_PAINT:
  {
    RECT WindowRect;
    GetClientRect(hWnd, &WindowRect);
    int screenX = WindowRect.right - WindowRect.left;
    int screenY = WindowRect.bottom - WindowRect.top;
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    HDC dcMemory = CreateCompatibleDC(NULL);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, screenX, screenY);
    SelectObject(dcMemory, bmp);

    QueryPerformanceCounter(&timer);
    __int64 record = timer.QuadPart;
    paintCore.paint(dcMemory, {&game2048, keyboardMode}, screenX, screenY, (record - lastRecord) / timerFrequency);
    lastRecord = record;

    BitBlt(hdc, 0, 0, screenX, screenY, dcMemory, 0, 0, SRCCOPY);
    DeleteObject(bmp);
    DeleteDC(dcMemory);
    ReleaseDC(hWnd, hdc);
    EndPaint(hWnd, &ps);
    break;
  }

  case WM_TIMER:
    if (wParam == 1) {
      InvalidateRect(hWnd, NULL, 0);
    }
    else if (!keyboardMode) {
      automation2048.singleMove();
    }
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}