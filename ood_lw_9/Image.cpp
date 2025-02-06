#include "Image.h"
#include <cassert>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iostream>

//сolor -> backgroundColor
Image::Image(Size size, uint32_t color) :
        m_size(size)
{
    if (size.height <= 0 || size.width <= 0)
    {
        throw std::out_of_range("Cannot create image less than 1 px width and height");
    }

    const int tilesInColumn = (size.width + Tile::SIZE - 1) / Tile::SIZE;
    const int tilesInRow = (size.height + Tile::SIZE - 1) / Tile::SIZE;

    m_tiles = std::vector<std::vector<CoW<Tile>>>
            (
                tilesInColumn, std::vector<CoW<Tile>>(tilesInRow, CoW(Tile(color)))
            );
}

Size Image::GetSize() const noexcept
{
    return m_size;
}


uint32_t Image::GetPixel(Point p) const noexcept
{
    if (p.x < 0 || p.x > m_size.width || p.y < 0 || p.y > m_size.height)
    {
        throw std::out_of_range("Out of range image pixel position");
    }
    const int tileColumIndex = p.x / Tile::SIZE;
    const int pixelColumIndex = p.x % Tile::SIZE;
    const int tileRowIndex = p.y / Tile::SIZE;
    const int pixelRowIndex = p.y % Tile::SIZE;

    return m_tiles[tileColumIndex][tileRowIndex]->GetPixel({pixelColumIndex, pixelRowIndex});
}

void Image::SetPixel(Point p, uint32_t color)
{
    if (p.x < 0 || p.x > m_size.width || p.y < 0 || p.y > m_size.height)
    {
        throw std::out_of_range("Out of range image pixel position");
    }

    const int tileColumIndex = p.x / Tile::SIZE;
    const int pixelColumIndex = p.x % Tile::SIZE;
    const int tileRowIndex = p.y / Tile::SIZE;
    const int pixelRowIndex = p.y % Tile::SIZE;

    auto oldColor = m_tiles[tileColumIndex][tileRowIndex]->GetPixel({pixelColumIndex, pixelRowIndex});
    m_tiles[tileColumIndex][tileRowIndex].Write()->SetPixel({pixelColumIndex, pixelRowIndex}, color ^ oldColor);
}