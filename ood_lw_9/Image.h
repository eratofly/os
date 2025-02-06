#pragma once
#include <iosfwd>
#include <string>

#include "geom.h"
#include "tile.h"
#include "cow.h"

class Image
{
public:
	explicit Image(Size size, uint32_t color = ' ');
	Size GetSize() const noexcept;
	uint32_t GetPixel(Point p) const noexcept;
	void SetPixel(Point p, uint32_t color);

private:
    Size m_size;
    std::vector<std::vector<CoW<Tile>>> m_tiles;
};
