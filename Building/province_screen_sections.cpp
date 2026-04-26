// File: province_screen_sections.cpp
// Commit: Update normal buildings panel text to match the unified standalone build menu and keep attachment restrictions on attachment widgets only

#include "province_screen_sections.h"
#include "province_screen_capital_render.h"
#include "province_screen_layout.h"
#include "ui.h"

namespace
{
    std::string resourcesSummaryText()
    {
        return
            "No resources added yet.\n"
            "Outputs: 0\n"
            "Inputs: 0\n"
            "Net: 0";
    }

    std::string metricsSummaryText()
    {
        return
            "Convoy Power: 0\n"
            "Caravan Power: 0\n"
            "Consumption Capacity: 0\n"
            "Lending Capacity: 0";
    }
}

void drawSectionTitle(SDL_Renderer* renderer, TTF_Font* sectionFont, const SDL_Rect& rect, const std::string& title)
{
    drawText(renderer, sectionFont, title, rect.x + kSectionPad, rect.y + 10, UiPalette::text);
}

void drawPopCountSection(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SectionLayout& popSection,
    const Province& province)
{
    drawWrappedText(
        renderer,
        bodyFont,
        "Peasants: " + std::to_string(province.totalPeasants()) +
            "\nFree Peasants: " + std::to_string(province.freePeasants()),
        popSection.rect.x + kSectionPad,
        popSection.rect.y + 54,
        popSection.rect.w - (kSectionPad * 2),
        UiPalette::text
    );
}

void drawNormalBuildingsSection(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SectionLayout& normalSection,
    const BuildDrawerState& drawer,
    std::vector<ClickTarget>& clickTargets)
{
    const std::string message =
        "This menu now opens the full standalone building list. "
        "Only attachment widgets should restrict the menu to attachment-valid buildings. "
        "City- and church-only attached building types are not added yet.";

    const int textHeight = measureWrappedHeight(
        bodyFont,
        message,
        normalSection.rect.w - (kSectionPad * 2),
        22
    );

    drawWrappedText(
        renderer,
        bodyFont,
        message,
        normalSection.rect.x + kSectionPad,
        normalSection.rect.y + 54,
        normalSection.rect.w - (kSectionPad * 2),
        UiPalette::mutedText
    );

    SDL_Rect addRect{
        normalSection.rect.x + kSectionPad,
        normalSection.rect.y + 54 + textHeight + 18,
        normalSection.rect.w - (kSectionPad * 2),
        kSlotHeight
    };
    drawAddRowButton(renderer, bodyFont, addRect, drawer.contextType == BuildContextType::normal_building_section);

    ClickTarget target;
    target.rect = addRect;
    target.contextType = BuildContextType::normal_building_section;
    target.targetBuildingId = -1;
    clickTargets.push_back(target);
}

void drawResourcesSection(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SectionLayout& resourcesSection)
{
    drawWrappedText(
        renderer,
        bodyFont,
        resourcesSummaryText(),
        resourcesSection.rect.x + kSectionPad,
        resourcesSection.rect.y + 54,
        resourcesSection.rect.w - (kSectionPad * 2),
        UiPalette::text
    );
}

void drawInfrastructureSection(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SectionLayout& infraSection)
{
    drawWrappedText(
        renderer,
        bodyFont,
        "No infrastructure building types added yet.",
        infraSection.rect.x + kSectionPad,
        infraSection.rect.y + 54,
        infraSection.rect.w - (kSectionPad * 2),
        UiPalette::mutedText
    );
}

void drawMetricsSection(
    SDL_Renderer* renderer,
    TTF_Font* bodyFont,
    const SectionLayout& metricsSection)
{
    drawWrappedText(
        renderer,
        bodyFont,
        metricsSummaryText(),
        metricsSection.rect.x + kSectionPad,
        metricsSection.rect.y + 54,
        metricsSection.rect.w - (kSectionPad * 2),
        UiPalette::text
    );
}