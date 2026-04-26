// File: province_screen_drawer.h
// Commit: Keep drawer header to declarations only and remove duplicate function definitions

#pragma once

#include "province.h"
#include "province_screen_types.h"
#include <string>
#include <vector>

std::vector<BuildOption> buildOptionsForContext(const Province& province, const BuildDrawerState& drawer);

void openRootDrawer(BuildDrawerState& drawer);
void openDirectAttachmentDrawer(BuildDrawerState& drawer, const Province& province, int targetBuildingId);
void openEstateAttachmentDrawer(BuildDrawerState& drawer, const Province& province, int targetBuildingId);
void openNormalBuildingsDrawer(BuildDrawerState& drawer);

bool applyDrawerBuildSelection(
    Province& province,
    const BuildDrawerState& drawer,
    const BuildOption& option,
    std::string& resultText
);