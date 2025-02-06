#include "ImageService.h"
#include <fstream>
#include <sstream>

void ImageService::SaveImage(const Image &img, const std::string &fileName) {
    std::ofstream file(fileName);
    if (file.is_open())
    {
        Print(img, file);
        file.close();
        std::string newFileName = fileName;
        std::string command = "pnmtopng " + fileName + " > " + "new_" + newFileName.erase(fileName.length() - 4) + ".png";
        system(command.c_str());
        std::cout << "The image saved as " << fileName << std::endl;
    }
    else
    {
        std::cerr << "Unable to create or open  file" << std::endl;
    }
}

Image ImageService::LoadImage(const std::string &pixels)
{
    std::istringstream s(pixels);
    Size size;
    std::string line;
    while (std::getline(s, line))
    {
        size.width = std::max(size.width, static_cast<int>(line.length()));
        ++size.height;
    }

    Image img(size);

    s = std::istringstream(pixels);
    for (int y = 0; y < size.height; ++y)
    {
        if (!std::getline(s, line))
        {
            break;
        }

        int x = 0;
        for (uint32_t ch: line)
        {
            img.SetPixel({x++, y}, ch);
        }
    }

    return img;
}

void ImageService::Print(const Image &img, std::ostream &out)
{
    const auto size = img.GetSize();
    out << "P3\n" << size.width << " " << size.height << "\n255\n";

    for (int y = 0; y < size.height; ++y)
    {
        for (int x = 0; x < size.width; ++x)
        {
            uint32_t color = img.GetPixel({x, y});
            uint8_t r = (color >> 16) & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = color & 0xFF;
            out << static_cast<int>(r) << " " << static_cast<int>(g) << " " << static_cast<int>(b) << " ";
        }
        out << "\n";
    }
}