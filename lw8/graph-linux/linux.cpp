#include <X11/Xlib.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <chrono>
#include <cmath>

#define WIDTH 800
#define HEIGHT 600
#define NUM_SNOWFLAKES 100
#define SNOWFLAKE_RADIUS_MIN 2
#define SNOWFLAKE_RADIUS_MAX 6
#define TARGET_FPS 60 // Целевая частота кадров

struct Snowflake
{
    float x, y;
    float ySpeed;  // скорость по вертикали (пиксели/секунду)
    float xOffset; // фаза синусоиды
    float xAmplitude; // амплитуда синусоиды
    int radius;
};

void InitSnowflakes(Snowflake snowflakes[], int count)
{
    for (int i = 0; i < count; i++)
    {
        snowflakes[i].x = rand() % WIDTH;
        snowflakes[i].y = rand() % HEIGHT - HEIGHT;
        snowflakes[i].ySpeed = 50 + rand() % 100; // Скорость от 50 до 150 пикселей/секунду
        snowflakes[i].xOffset = static_cast<float>(rand() % 360); // Начальная фаза синусоиды
        snowflakes[i].xAmplitude = 20 + rand() % 40;              // Амплитуда синусоиды
        snowflakes[i].radius = SNOWFLAKE_RADIUS_MIN + rand() % (SNOWFLAKE_RADIUS_MAX - SNOWFLAKE_RADIUS_MIN + 1);
    }
}

void UpdateSnowflakes(Snowflake snowflakes[], int count, float deltaTime)
{
    for (int i = 0; i < count; i++)
    {
        snowflakes[i].y += snowflakes[i].ySpeed * deltaTime;
        snowflakes[i].x += sin(snowflakes[i].xOffset) * snowflakes[i].xAmplitude * deltaTime;
        snowflakes[i].xOffset += deltaTime * 2; // Увеличиваем фазу для плавного движения

        // Перезапуск снежинки при выходе за нижнюю границу
        if (snowflakes[i].y > HEIGHT)
        {
            snowflakes[i].x = rand() % WIDTH;
            snowflakes[i].y = rand() % HEIGHT - HEIGHT;
            snowflakes[i].ySpeed = 50 + rand() % 100;
            snowflakes[i].xOffset = static_cast<float>(rand() % 360);
            snowflakes[i].xAmplitude = 20 + rand() % 40;
        }
    }
}

void DrawSnowflakes(Display *display, Window window, GC gc, Snowflake snowflakes[], int count)
{
    XClearWindow(display, window);
    for (int i = 0; i < count; i++)
    {
        XFillArc(display, window, gc,
                 static_cast<int>(snowflakes[i].x) - snowflakes[i].radius,
                 static_cast<int>(snowflakes[i].y) - snowflakes[i].radius,
                 snowflakes[i].radius * 2,
                 snowflakes[i].radius * 2,
                 0, 360 * 64);
    }
}

class DrawingX11
{
private:
    Display *display;
    Window window;
    GC gc;
    int screen;

public:
    DrawingX11(int width, int height)
    {
        display = XOpenDisplay(nullptr);
        if (!display)
        {
            throw std::runtime_error("Не удалось открыть дисплей.");
        }

        screen = DefaultScreen(display);
        Window root = RootWindow(display, screen);

        window = XCreateSimpleWindow(display, root, 0, 0, width, height, 1, BlackPixel(display, screen), BlackPixel(display, screen));
        XSelectInput(display, window, ExposureMask | KeyPressMask);
        XMapWindow(display, window);

        gc = XCreateGC(display, window, 0, nullptr);
        XSetForeground(display, gc, WhitePixel(display, screen));
    }

    ~DrawingX11()
    {
        if (gc)
            XFreeGC(display, gc);
        if (window)
            XDestroyWindow(display, window);
        if (display)
            XCloseDisplay(display);
    }

    Display *getDisplay() const { return display; }
    Window getWindow() const { return window; }
    GC getGC() const { return gc; }
};

int main()
{
    try
    {
        DrawingX11 app(WIDTH, HEIGHT);

        srand(static_cast<unsigned>(time(nullptr)));
        Snowflake snowflakes[NUM_SNOWFLAKES];
        InitSnowflakes(snowflakes, NUM_SNOWFLAKES);

        auto lastTime = std::chrono::high_resolution_clock::now();
        const float frameDuration = 1.0f / TARGET_FPS;

        XEvent event;
        while (true)
        {
            if (XPending(app.getDisplay()))
            {
                XNextEvent(app.getDisplay(), &event);
                if (event.type == KeyPress)
                {
                    break;
                }
            }

            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> elapsed = currentTime - lastTime;
            if (elapsed.count() < frameDuration)
            {
                continue; // Ждём до следующего кадра
            }
            lastTime = currentTime;

            float deltaTime = elapsed.count();

            UpdateSnowflakes(snowflakes, NUM_SNOWFLAKES, deltaTime);
            DrawSnowflakes(app.getDisplay(), app.getWindow(), app.getGC(), snowflakes, NUM_SNOWFLAKES);

            XFlush(app.getDisplay());
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
