#pragma once
#include "Image.h"
#include <fstream>

class ImageService
{
public:
    static Image LoadImage(const std::string& pixels);
    static void SaveImage(const Image& img, const std::string& fileName);

private:
    static void Print(const Image& img, std::ostream& out);
};