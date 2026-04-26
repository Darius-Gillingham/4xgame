// File: province_screen_types.h
// Commit: Extract province screen internal UI state types for drawer, click targets, and section layout

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