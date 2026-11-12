// File: building/province_screen_types.h
// Commit: Add the Army tab to the live province overlay state

#pragma once

#include "building.h"

#include <SDL2/SDL.h>

#include <string>

enum class BuildContextType
{
    none,
    root_holding,
    direct_attachment_slot,
    estate_attachment_slot,
    building_slot,
    building_detail,
    normal_holding_section
};

enum class ProvinceOverlayTab
{
    holdings,
    population,
    army
};

enum class MapViewMode
{
    terrain,
    regions,
    provinces
};

enum class EscapeMenuAction
{
    none,
    close_menu,
    save_game,
    load_game,
    exit_game
};

struct ClickTarget
{
    SDL_Rect rect{};

    BuildContextType contextType =
        BuildContextType::none;

    int targetHoldingId = -1;
    int targetBuildingId = -1;
};

struct ProvinceOverlayState
{
    bool open = false;

    ProvinceOverlayTab activeTab =
        ProvinceOverlayTab::holdings;

    int scrollOffset = 0;

    int selectedHoldingId = -1;
    int selectedBuildingId = -1;

    bool showingHoldingDetails = false;

    SDL_Rect selectedHoldingRect{};
    SDL_Rect selectedBuildingRect{};
};

struct MapShellState
{
    bool escapeMenuOpen = false;

    MapViewMode activeMapMode =
        MapViewMode::terrain;

    int gameSpeed = 1;

    int monthProgress = 0;
    int monthProgressTarget = 100;

    std::string statusText =
        "Months advance automatically.";
};