// File: province_screen_types.h
// Commit: Add sub-building slot drawer context to province screen state

#pragma once

#include "building.h"
#include <SDL2/SDL.h>
#include <string>

enum class BuildContextType
{
    none,
    root_capital,
    direct_attachment_slot,
    estate_attachment_slot,
    sub_building_slot,
    normal_building_section
};

struct BuildDrawerState
{
    BuildContextType contextType = BuildContextType::none;
    int targetBuildingId = -1;
    int scrollOffset = 0;
    std::string title = "Building Choice";
    std::string subtitle = "Select a + widget to choose what to build.";
};

struct BuildOption
{
    std::string label;
    std::string description;
    bool enabled = false;
    BuildingType buildingType = BuildingType::castle;
    SubBuildingType subBuildingType = SubBuildingType::game_hunters;
};

struct ClickTarget
{
    SDL_Rect rect{};
    BuildContextType contextType = BuildContextType::none;
    int targetBuildingId = -1;
};

struct SectionLayout
{
    SDL_Rect rect{};
    int contentHeight = 0;
};