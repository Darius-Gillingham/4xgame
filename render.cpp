// File: render.cpp
// Commit: Remove A suffixes from PPM renderer types and function names

#include "render.h"
#include <fstream>

struct Color
{
    int r;
    int g;
    int b;
};

static Color color(Terrain t)
{
    switch (t)
    {
        case Terrain::tundra: return {235, 235, 225};
        case Terrain::boreal: return {24, 52, 28};
        case Terrain::woods: return {42, 78, 40};
        case Terrain::highlands: return {108, 126, 104};
        case Terrain::hills: return {140, 104, 72};
        case Terrain::mountains: return {140, 140, 140};
        case Terrain::plains: return {168, 186, 92};
        case Terrain::farmlands: return {92, 210, 84};
        case Terrain::marsh: return {120, 188, 168};
        case Terrain::steppe: return {210, 206, 120};
        case Terrain::arid: return {186, 150, 104};
        case Terrain::desert: return {226, 190, 92};
        case Terrain::ocean: return {18, 46, 110};
        default: return {255, 0, 255};
    }
}

bool renderToPpm(const char* filePath, const Map& map)
{
    std::ofstream out(filePath, std::ios::out);
    if (!out.is_open()) return false;

    out << "P3\n" << map.width() << " " << map.height() << "\n255\n";

    for (int y = 0; y < map.height(); ++y)
    {
        for (int x = 0; x < map.width(); ++x)
        {
            Color c = color(map.at(x, y).terrain);
            out << c.r << " " << c.g << " " << c.b << " ";
        }
        out << "\n";
    }

    return true;
}