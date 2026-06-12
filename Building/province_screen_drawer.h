// File: province_screen_drawer.h
// Commit: Add sub-building drawer open helper declaration

#pragma once

#include "province.h"
#include "province_screen_types.h"
#include <string>
#include <vector>

std::vector<BuildOption> buildOptionsForContext(const Province& province, const BuildDrawerState& drawer);

void openRootDrawer(BuildDrawerState& drawer);
void openDirectAttachmentDrawer(BuildDrawerState& drawer, const Province& province, int targetBuildingId);
void openEstateAttachmentDrawer(BuildDrawerState& drawer, const Province& province, int targetBuildingId);
void openSubBuildingDrawer(BuildDrawerState& drawer, const Province& province, int targetBuildingId);
void openNormalBuildingsDrawer(BuildDrawerState& drawer);

bool applyDrawerBuildSelection(
    Province& province,
    const BuildDrawerState& drawer,
    const BuildOption& option,
    std::string& resultText
);