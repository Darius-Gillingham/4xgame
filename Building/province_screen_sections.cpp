// File: building/province_screen_sections.cpp
// Commit: Replace removed peasant counters with current population counters

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
        "Population: " + std::to_string(province.totalPopulation()) +
            "\nUnassigned Population: " + std::to_string(province.unassignedPopulation()),
        popSection.rect.x + kSectionPad,
        popSection.rect.y + 54,
        popSection.rect.w - (kSectionPad * 2),
        UiPalette::text
    );
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