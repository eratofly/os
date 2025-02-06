#include "drawer.h"
#include <cstdlib>
#include <cassert>

namespace
{

    //??? ??? Sign
//1, ???? to.y > from.y (????? ??????????? ?????),
//-1, ???? to.y < from.y (????? ?????????? ????),
//0, ???? to.y == from.y (????? ?????????????).

    int Sign(int value)
    {
        return (0 < value) - (value < 0);
    }

    void DrawSteepLine(Image &image, Point from, Point to, uint32_t color)
    {
        const int deltaX = std::abs(to.x - from.x);
        const int deltaY = std::abs(to.y - from.y);

        assert(deltaY >= deltaX);

        if (from.y > to.y)
        {
            std::swap(from, to);
        }

        const int stepX = Sign(to.x - from.x);
        const int errorThreshold = deltaY + 1;
        const int deltaErr = deltaX + 1;

        int error = deltaErr / 2;

        for (Point p = from; p.y <= to.y; ++p.y)
        {
            image.SetPixel({p.x, p.y}, color);
            assert((p.y != to.y) || (p.x == to.x));

            error += deltaErr;

            if (error >= errorThreshold)
            {
                p.x += stepX;
                error -= errorThreshold;
            }
        }
    }

    void DrawSlopeLine(Image &image, Point from, Point to, uint32_t color)
    {
        const int deltaX = std::abs(to.x - from.x);
        const int deltaY = std::abs(to.y - from.y);

        assert(deltaX >= deltaY);

        if (from.x > to.x)
        {
            std::swap(from, to);
        }

        const int stepY = Sign(to.y - from.y);
        std::cout << "stepY: " << stepY << std::endl;
        const int errorThreshold = deltaX + 1;
        std::cout << "errorThreshold: " << errorThreshold << std::endl;
        const int deltaErr = deltaY + 1;
        std::cout << "deltaErr: " << deltaErr << std::endl;

        int error = deltaErr / 2;
        std::cout << "error: " << error << std::endl;


        for (Point p = from; p.x <= to.x; ++p.x)
        {
            image.SetPixel({p.x, p.y}, color);
            assert((p.x != to.x) || (p.y == to.y));

            error += deltaErr;

            if (error >= errorThreshold)
            {
                p.y += stepY;
                error -= errorThreshold;
            }
        }
    }

} // namespace

void DrawLine(Image &image, Point from, Point to, uint32_t color)
{
    const int deltaX = std::abs(to.x - from.x);
    const int deltaY = std::abs(to.y - from.y);

    if (deltaY > deltaX)
    {
        DrawSteepLine(image, from, to, color);
    } else
    {
        DrawSlopeLine(image, from, to, color);
    }
}

void DrawCirclePoints(Image &image, Point center, int x, int y, uint32_t color)
{
    image.SetPixel({center.x + x, center.y + y}, color);
    image.SetPixel({center.x - x, center.y + y}, color);
    image.SetPixel({center.x + x, center.y - y}, color);
    image.SetPixel({center.x - x, center.y - y}, color);
    image.SetPixel({center.x + y, center.y + x}, color);
    image.SetPixel({center.x - y, center.y + x}, color);
    image.SetPixel({center.x + y, center.y - x}, color);
    image.SetPixel({center.x - y, center.y - x}, color);
}

void DrawCircle(Image &image, Point center, int radius, uint32_t color)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y)
    {
        DrawCirclePoints(image, center, x, y, color);

        if (d < 0)
        {
            d += 4 * x + 6;
        } else
        {
            d += 4 * (x - y) + 10;
            --y;
        }

        ++x;
    }
}

void FillCircleLines(Image &image, Point center, int x, int y, uint32_t color)
{
    for (int i = -x; i <= x; ++i)
    {
        image.SetPixel({center.x + i, center.y + y}, color);
        image.SetPixel({center.x + i, center.y - y}, color);
    }
}

void FillCircle(Image &image, Point center, int radius, uint32_t color)
{
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y)
    {
        FillCircleLines(image, center, x, y, color);
        FillCircleLines(image, center, y, x, color);

        if (d < 0)
        {
            d += 4 * x + 6;
        } else
        {
            d += 4 * (x - y) + 10;
            --y;
        }

        ++x;
    }
}
