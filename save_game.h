// File: save_game.h
// Commit: Add saved country discovery state

#pragma once

#include "game.h"
#include "map.h"

#include <cstdint>
#include <string>
#include <vector>

struct SavedTile
{
    bool land = false;
    Terrain terrain = Terrain::ocean;
    Region region = Region::none;
    int polityId = -1;
    int provinceId = -1;
};

struct SavedGameData
{
    int mapWidth = 0;
    int mapHeight = 0;
    std::uint32_t mapSeed = 0;
    std::vector<SavedTile> tiles;

    PlayerCounty playerCounty;
    int tickCount = 0;
    GameDate date;
    ProvinceSaveData provinceData;
    std::vector<Country> countries;
};

bool saveGameToFile(const std::string& filePath, const Map& map, const GameState& gameState, std::string& reason);
bool loadGameFromFile(const std::string& filePath, SavedGameData& savedGameData, std::string& reason);