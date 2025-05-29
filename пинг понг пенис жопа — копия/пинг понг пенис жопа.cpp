// Пинг-Понг.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Resource.h"
#include <conio.h>


#define MAX_LOADSTRING 1000
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 150
#define BALL_SIZE 20
#define PADDLE_SPEED 30

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Игровые переменные
RECT paddleLeft, paddleRight, ball;
int ballSpeedX = 3, ballSpeedY = 3;
int player1Score = 0, player2Score = 0;
bool gamePaused = true;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                UpdateGame(HWND hWnd);
void                DrawGame(HDC hdc, HWND hWnd);
void                ResetBall(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY));

    MSG msg;

    // Основной цикл сообщений:
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
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, L"Пинг-Понг", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }



    // Инициализация игровых объектов
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    int paddleY = (clientRect.bottom - clientRect.top - PADDLE_HEIGHT) / 2;

    paddleLeft.left = 20;
    paddleLeft.top = paddleY;
    paddleLeft.right = paddleLeft.left + PADDLE_WIDTH;
    paddleLeft.bottom = paddleLeft.top + PADDLE_HEIGHT;

    paddleRight.right = clientRect.right - 20;
    paddleRight.left = paddleRight.right - PADDLE_WIDTH;
    paddleRight.top = paddleY;
    paddleRight.bottom = paddleRight.top + PADDLE_HEIGHT;

    ResetBall(hWnd);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Установка таймера для обновления игры
    SetTimer(hWnd, 1, 1, NULL); // ~60 FPS

    return TRUE;
}




void ResetBall(HWND hWnd)
{
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    int centerX = (clientRect.right - clientRect.left) / 2 - BALL_SIZE / 2;
    int centerY = (clientRect.bottom - clientRect.top) / 2 - BALL_SIZE / 2;

    ball.left = centerX;
    ball.top = centerY;
    ball.right = ball.left + BALL_SIZE;
    ball.bottom = ball.top + BALL_SIZE;

    // Случайное направление мяча
    ballSpeedX = (rand() % 20 == 0) ? 3 : -3;
    ballSpeedY = (rand() % 5) - 2; // От -2 до 2

    int paddleY = (clientRect.bottom - clientRect.top - PADDLE_HEIGHT) / 2;

    paddleLeft.left = 20;
    paddleLeft.top = paddleY;
    paddleLeft.right = paddleLeft.left + PADDLE_WIDTH;
    paddleLeft.bottom = paddleLeft.top + PADDLE_HEIGHT;

    paddleRight.right = clientRect.right - 20;
    paddleRight.left = paddleRight.right - PADDLE_WIDTH;
    paddleRight.top = paddleY;
    paddleRight.bottom = paddleRight.top + PADDLE_HEIGHT;
}

void UpdateGame(HWND hWnd)
{
    if (gamePaused) return;

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    // Движение мяча
    ball.left += ballSpeedX;
    ball.top += ballSpeedY;
    ball.right = ball.left + BALL_SIZE;
    ball.bottom = ball.top + BALL_SIZE;

    // Столкновение с верхней и нижней границами
    if (ball.top <= clientRect.top || ball.bottom >= clientRect.bottom)
    {
        ballSpeedY = -ballSpeedY;
    }

    // Столкновение с левой ракеткой
    if (ball.left <= paddleLeft.right &&
        ball.right >= paddleLeft.left &&
        ball.bottom >= paddleLeft.top &&
        ball.top <= paddleLeft.bottom)
    {
        ballSpeedX = abs(ballSpeedX); // Гарантированно двигаем вправо
        // Угол отскока зависит от места удара по ракетке
        int hitPos = (ball.top + BALL_SIZE / 2) - paddleLeft.top;
        ballSpeedY = (hitPos - PADDLE_HEIGHT / 2) / 10;
    }

    // Столкновение с правой ракеткой
    if (ball.right >= paddleRight.left &&
        ball.left <= paddleRight.right &&
        ball.bottom >= paddleRight.top &&
        ball.top <= paddleRight.bottom)
    {
        ballSpeedX = -abs(ballSpeedX); // Гарантированно двигаем влево
        // Угол отскока зависит от места удара по ракетке
        int hitPos = (ball.top + BALL_SIZE / 2) - paddleRight.top;
        ballSpeedY = (hitPos - PADDLE_HEIGHT / 2) / 10;
    }

    // Гол за левую ракетку
    if (ball.left <= clientRect.left)
    {
        player2Score++;
        ResetBall(hWnd);
        gamePaused = true;
    }

    // Гол за правую ракетку
    if (ball.right >= clientRect.right)
    {
        player1Score++;
        ResetBall(hWnd);
        gamePaused = true;
    }

    // _getch() ожидает нажатия клавиши без необходимости нажимать Enter
    int key = _getch();

    // Проверяем, что нажата 'ё' (код 184 в Windows для русской раскладки)
    if (key == 184) {
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        int paddleY = (clientRect.bottom - clientRect.top - PADDLE_HEIGHT) / 2;

        paddleLeft.left = 20;
        paddleLeft.top = paddleY;
        paddleLeft.right = paddleLeft.left + PADDLE_WIDTH;
        paddleLeft.bottom = paddleLeft.top + PADDLE_HEIGHT;

        paddleRight.right = clientRect.right - 20;
        paddleRight.left = paddleRight.right - PADDLE_WIDTH;
        paddleRight.top = paddleY;
        paddleRight.bottom = paddleRight.top + PADDLE_HEIGHT;

        ResetBall(hWnd);

    
        UpdateWindow(hWnd);
        // Здесь можно добавить нужную логику
    }



    InvalidateRect(hWnd, NULL, FALSE);
}

void DrawGame(HDC hdc, HWND hWnd)
{
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    // Очистка экрана
    HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
    FillRect(hdc, &clientRect, hBrush);
    DeleteObject(hBrush);

    // Рисуем разделительную линию
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    for (int y = 0; y < clientRect.bottom; y += 20)
    {
        MoveToEx(hdc, clientRect.right / 2, y, NULL);
        LineTo(hdc, clientRect.right / 2, y + 10);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    // Рисуем ракетки
    hBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdc, &paddleLeft, hBrush);
    FillRect(hdc, &paddleRight, hBrush);
    DeleteObject(hBrush);

    // Рисуем мяч
    hBrush = CreateSolidBrush(RGB(255,255,255));
    FillRect(hdc, &ball, hBrush);
    DeleteObject(hBrush);

    // Рисуем счет
    SetTextColor(hdc, RGB(0, 255, 0));
    SetBkMode(hdc, TRANSPARENT);

    WCHAR scoreText[50];
    wsprintf(scoreText, L"%d - %d", player1Score, player2Score);

    RECT textRect;
    textRect.left = clientRect.right / 2 - 50;
    textRect.top = 20;
    textRect.right = clientRect.right / 2 + 50;
    textRect.bottom = 60;

    DrawText(hdc, scoreText, -1, &textRect, DT_CENTER | DT_VCENTER);

    // Сообщение о паузе
    if (gamePaused)
    {
        DrawText(hdc, L"Нажмите ПРОБЕЛ для начала игры", -1, &clientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_KEYDOWN:
    {
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        switch (wParam)
        {
        case VK_SPACE:
            gamePaused = !gamePaused;
            InvalidateRect(hWnd, NULL, FALSE);
            break;

            // Управление левой ракеткой (W/S)
        case 'W':
            if (paddleLeft.top > clientRect.top)
            {
                paddleLeft.top -= PADDLE_SPEED;
                paddleLeft.bottom -= PADDLE_SPEED;
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
        case 'S':
            if (paddleLeft.bottom < clientRect.bottom)
            {
                paddleLeft.top += PADDLE_SPEED;
                paddleLeft.bottom += PADDLE_SPEED;
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;

            // Управление правой ракеткой (стрелки вверх/вниз)
        case VK_UP:
            if (paddleRight.top > clientRect.top)
            {
                paddleRight.top -= PADDLE_SPEED;
                paddleRight.bottom -= PADDLE_SPEED;
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
        case VK_DOWN:
            if (paddleRight.bottom < clientRect.bottom)
            {
                paddleRight.top += PADDLE_SPEED;
                paddleRight.bottom += PADDLE_SPEED;
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
        }
    }
    break;

    case WM_TIMER:
        UpdateGame(hWnd);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        DrawGame(hdc, hWnd);
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}