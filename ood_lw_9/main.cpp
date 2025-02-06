#include <cmath>
#include <iostream>
#include <fstream>

#include "drawer.h"
#include "ImageService.h"

constexpr int STAR_COUNT = 20;

int main()
{
    try
    {
        {
            Image img = ImageService::LoadImage(
                    " CCCC             \n"
                    "CC  CC   FF    FF \n"
                    "CC      FFFF  FFFF\n"
                    "CC  CC   FF    FF \n"
                    " CCCC             \n");

            ImageService::SaveImage(img, "output1.ppm");
        }

        {
            Image img{ { 30, 20 }, 0x00FF00 };

            DrawLine(img, { 3, 2 }, { 26, 5 }, 0xFF0000);
            DrawLine(img, { 26, 5 }, { 21, 18 }, 0xFF0000);
            DrawLine(img, { 21, 18 }, { 3, 2 }, 0xFF0000);

            ImageService::SaveImage(img, "output2.ppm");
        }

        {
            Image img{ { 600, 600 }, 0xFF0000 };

            FillCircle(img, {150, 150}, 100, 0xF0FF0F);
            DrawCircle(img, {300, 300}, 4, 0xFF0000);
            DrawCircle(img, {300, 300}, 8, 0x0FFF00);
            DrawCircle(img, {300, 300}, 16, 0x0FFFF0);
            DrawCircle(img, {300, 300}, 32, 0x00FF00);
            DrawCircle(img, {300, 300}, 64, 0x000FF0);
            DrawCircle(img, {300, 300}, 128, 0x0000FF);
            DrawCircle(img, {300, 300}, 256, 0x000000);

            ImageService::SaveImage(img, "output3.ppm");
        }

        {
            Image img{ { 500, 500 }, 0xFF0000 };

            FillCircle(img, {250, 250}, 50, 0x00FF00);

            ImageService::SaveImage(img, "output4.ppm");
        }
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
    }
}
