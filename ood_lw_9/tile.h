#pragma once

#include <array>
#include <cassert>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <memory>

#include "geom.h"

class Tile
{
public:
    constexpr static int SIZE = 8;

    explicit Tile(uint32_t color = ' ') noexcept
            : m_pixels(std::vector<std::vector<uint32_t>>(SIZE, std::vector<uint32_t>(SIZE, color)))
    {
        // -------------- не удалять ------------
        assert(m_instanceCount >= 0);
        ++m_instanceCount; // Увеличиваем счётчик тайлов (для целей тестирования).
        // -------------- не удалять ------------
    }

    // Конструктор копирования.
    Tile(const Tile &other)
            : m_pixels(other.m_pixels)
    {
        // -------------- не удалять ------------
        assert(m_instanceCount >= 0);
        ++m_instanceCount; // Увеличиваем счётчик тайлов (для целей тестирования).
        // -------------- не удалять ------------
    }

    // Конструктор перемещения.
    Tile(Tile &&other) noexcept
            : m_pixels(std::move(other.m_pixels))
    {
        // -------------- не удалять ------------
        assert(m_instanceCount >= 0);
        ++m_instanceCount; // Увеличиваем счётчик тайлов (для целей тестирования).
        // -------------- не удалять ------------
    }

    // Оператор присваивания копированием.
    Tile &operator=(const Tile &other)
    {
        if (this != &other)
        {
            m_pixels = other.m_pixels;  // Копируем вектор пикселей
        }
        return *this;
    }

    // Оператор присваивания перемещением.
    Tile &operator=(Tile &&other) noexcept
    {
        if (this != &other)
        {
            m_pixels = std::move(other.m_pixels);  // Перемещаем вектор пикселей
        }
        return *this;
    }

    ~Tile()
    {
        // -------------- не удалять ------------
        --m_instanceCount; // Уменьшаем счётчик тайлов.
        assert(m_instanceCount >= 0);
        // -------------- не удалять ------------
    }

    void SetPixel(Point p, uint32_t color)
    {
        if (p.x < 0 || p.x >= SIZE || p.y < 0 || p.y >= SIZE)
        {
            throw std::out_of_range("Addressing out of range tile pixel");
        }
        m_pixels[p.x][p.y] = color;
    }

    uint32_t GetPixel(Point p) const
    {
        if (p.x < 0 || p.x >= SIZE || p.y < 0 || p.y >= SIZE)
        {
            throw std::out_of_range("Addressing out of range tile pixel");
        }
        return m_pixels[p.x][p.y];
    }

    // Возвращает количество экземпляра класса Tile в программе.
    static int GetInstanceCount() noexcept
    {
        // -------------- не удалять ------------
        return m_instanceCount;
        // -------------- не удалять ------------
    }

private:
    // -------------- не удалять ------------
    inline static int m_instanceCount = 0;
    // -------------- не удалять ------------

    std::vector<std::vector<uint32_t>> m_pixels;
};
