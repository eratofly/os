#include <windows.h>
#include <tchar.h>
#include <cstdlib>
#include <ctime>
#include <memory>

class Brush
{
public:
    explicit Brush(COLORREF color)
    {
        m_hBrush = CreateSolidBrush(color);
    }

    //доопередилить, добавить m c подчёркиванием

    ~Brush()
    {
        if (m_hBrush)
        {
            DeleteObject(m_hBrush);
        }
    }

    void Reset(COLORREF color)
    {
        if (m_hBrush)
        {
            DeleteObject(m_hBrush);
        }
        m_hBrush = CreateSolidBrush(color);
    }

    HBRUSH Get() const
    {
        return m_hBrush;
    }

private:
    HBRUSH m_hBrush;
};

COLORREF GetRandomColor()
{
    return RGB(rand() % 256, rand() % 256, rand() % 256);
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    srand(static_cast<unsigned int>(time(0)));
//что такое wndclass
//wndclass - структура, которая описывает класс окна
//lpfnWndProc - Указатель на процедуру окна. Для вызова процедуры окна необходимо использовать функцию CallWindowProc .
//hInstance - Дескриптор экземпляра , содержащего процедуру окна для класса .
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("EllipseApp");
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClass(&wc))
    {
        MessageBox(nullptr, _T("Ошибка регистрации класса окна"), _T("Ошибка"), MB_ICONERROR);
        return 0;
    }

// создание окна
//WS_OVERLAPPEDWINDOW - нужно знать, что значит каждая переменная
//Константа WS_OVERLAPPED определяет базовый стиль окна - перекрывающееся окно.
// Стиль WS_OVERLAPPEDWINDOW в добавление к базовому указывает, что окно должно иметь заголовок (константа WS_CAPTION), системное меню (WS_SYSMENU),
// толстую рамку для изменения размера окна (WS_THICKFRAME), кнопку минимизации размера окна (WS_MINIMIZEBOX) и кнопку для максимального увеличения размера окна (WS_MAXIMIZEBOX).
// Если окно имеет заголовок, вы можете его перемещать по экрану.
//CW_USEDEFAULT - система разместит окно по своему усмотрению
    HWND hWnd = CreateWindow(
            _T("EllipseApp"), _T("Ellipse"),
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
            nullptr, nullptr, hInstance, nullptr
    );

    if (!hWnd)
    {
        MessageBox(nullptr, _T("Не удалось создать окно"), _T("Ошибка"), MB_ICONERROR);
        return 0;
    }
//nCmdShow - что это?
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG message = {};
    //цикл выборки сообщений
    while (GetMessage(&message, nullptr, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    //GWLP_USERDATA
    //GetWindowLongPtr и SetWindowLongPtr

    return static_cast<int>(message.wParam);
}

// обработчик сообщений окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //в оконной процедуре не использовать переменные
    static std::unique_ptr<Brush> brush = std::make_unique<Brush>(RGB(255, 0, 0));
    static HMENU hMenu;
    static RECT clientRect;

    switch (message)
    {
        case WM_CREATE:
        {
// создание меню
            hMenu = CreateMenu();
            HMENU hFileMenu = CreateMenu();
            AppendMenu(hFileMenu, MF_STRING, 1, _T("Exit"));
            AppendMenu(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hFileMenu), _T("File"));
            SetMenu(hWnd, hMenu);
            break;
        }

        case WM_SIZE:
        {
// обновление размеров окна
            GetClientRect(hWnd, &clientRect);
            InvalidateRect(hWnd, nullptr, TRUE);
            break;
        }

        case WM_LBUTTONDOWN:
        {
// проверка клика внутри эллипса
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            HRGN hRegion = CreateEllipticRgnIndirect(&clientRect);
            if (PtInRegion(hRegion, x, y))
            {
                brush->Reset(GetRandomColor());
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            DeleteObject(hRegion);
            break;
        }

        case WM_COMMAND:
        {
// обработка команд меню
            switch (LOWORD(wParam))
            {
                case 1: // Exit
                    CloseWindow(hWnd);
                    break;
            }
            break;
        }

        case WM_PAINT:
        {
// рисование эллипса
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            SelectObject(hdc, brush->Get());
            Ellipse(hdc, clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);

            EndPaint(hWnd, &ps);
            break;
        }

        case WM_DESTROY:
        {
            //чем SendMessage отличается от PostQuitMessage
            PostQuitMessage(0);
            break;
        }

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
