// File: game.h
// Commit: Add player setup and county spawn declarations for the first playable county start

#pragma once
#include "map.h"
#include <cstdint>
#include <string>
#include <vector>

enum class Culture
{
    anglo_saxons,
    franks,
    frisians,
    norse,
    wends,
    saxons,
    goths,
    lombards,
    bulgars,
    magyars,
    byzantines,
    rus,
    poles
};

enum class Religion
{
    catholic,
    orthodox,
    pagan
};

enum class Government
{
    tribal,
    aristocratic,
    plutocratic,
    theocratic
};

enum class RulerTrait
{
    martial,
    builder,
    pious,
    trader,
    scholar,
    administrator
};

struct PlayerSetup
{
    Culture culture = Culture::anglo_saxons;
    Region startRegion = Region::british_isles;
    Religion religion = Religion::catholic;
    Government government = Government::tribal;
    std::string rulerName = "Ruler";
    int rulerAge = 30;
    RulerTrait primaryTrait = RulerTrait::martial;
    RulerTrait secondaryTrait = RulerTrait::builder;
};

struct PlayerCounty
{
    std::int32_t polityId = 0;
    int countyX = -1;
    int countyY = -1;
    PlayerSetup setup;
};

const char* cultureName(Culture culture);
const char* religionName(Religion religion);
const char* governmentName(Government government);
const char* rulerTraitName(RulerTrait trait);
const char* regionName(Region region);

const std::vector<Region>& allowedStartingRegions(Culture culture);
bool isAllowedStartingRegion(Culture culture, Region region);
Region defaultStartingRegion(Culture culture);

bool spawnPlayerCounty(Map& map, PlayerCounty& playerCounty);