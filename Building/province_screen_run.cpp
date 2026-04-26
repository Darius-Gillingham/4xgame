// File: province_screen_run.cpp
// Commit: Keep the drawer on the root building list by default and remove the root add widget from the main buildings panel

#include "province_screen.h"
#include "province_screen_capital_render.h"
#include "province_screen_drawer.h"
#include "province_screen_layout.h"
#include "province_screen_sections.h"
#include "province_screen_types.h"
#include "ui.h"

#include <algorithm>
#include <string>
#include <vector>

void runProvinceScreen(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* sharedFont, Province& province)
{
    (void)window;

    if (!renderer)
    {
        return;
    }

    TTF_Font* titleFont = loadFontOrNull(32);
    TTF_Font* sectionFont = loadFontOrNull(24);
    TTF_Font* bodyFont = loadFontOrNull(18);

    if (!titleFont)
    {
        titleFont = sharedFont;
    }

    if (!sectionFont)
    {
        sectionFont = sharedFont;
    }

    if (!bodyFont)
    {
        bodyFont = sharedFont;
    }

    BuildDrawerState drawer;
    openRootDrawer(drawer);

    bool running = true;
    int documentScrollOffset = 0;
    std::vector<ClickTarget> clickTargets;

    while (running)
    {
        int screenWidth = 0;
        int screenHeight = 0;
        SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

        const int viewportWidth = screenWidth - (kDrawerWidth + kColumnGap);
        const int contentLeft = kOuterPadding;
        const int contentTop = kOuterPadding;
        const int contentWidth = viewportWidth - (kOuterPadding * 2);
        const int leftColumnWidth = (contentWidth * 66) / 100;
        const int rightColumnWidth = contentWidth - leftColumnWidth - kColumnGap;
        const int leftX = contentLeft;
        const int rightX = leftX + leftColumnWidth + kColumnGap;

        const SDL_Rect headerRect{contentLeft, contentTop, contentWidth, kHeaderHeight};

        const int row1BoxHeight = std::max(
            capitalSectionContentHeight(province, bodyFont),
            popCountSectionContentHeight(bodyFont)
        ) + 52;
        const int row2BoxHeight = std::max(
            infrastructureSectionContentHeight(bodyFont),
            resourcesSectionContentHeight(bodyFont)
        ) + 52;
        const int row3BoxHeight = metricsSectionContentHeight(bodyFont) + 52;

        const int row1Y = contentTop + kHeaderHeight + kRowGap;
        const int row2Y = row1Y + row1BoxHeight + kRowGap;
        const int row3Y = row2Y + row2BoxHeight + kRowGap;

        const int documentContentHeight =
            kHeaderHeight + kRowGap +
            row1BoxHeight + kRowGap +
            row2BoxHeight + kRowGap +
            row3BoxHeight + kOuterPadding;

        const int maxDocumentScroll = std::max(0, documentContentHeight - screenHeight);
        documentScrollOffset = std::clamp(documentScrollOffset, 0, maxDocumentScroll);

        const SectionLayout buildingsSection{{leftX, row1Y - documentScrollOffset, leftColumnWidth, row1BoxHeight}, 0};
        const SectionLayout popSection{{rightX, row1Y - documentScrollOffset, rightColumnWidth, row1BoxHeight}, 0};
        const SectionLayout infraSection{{leftX, row2Y - documentScrollOffset, leftColumnWidth, row2BoxHeight}, 0};
        const SectionLayout resourcesSection{{rightX, row2Y - documentScrollOffset, rightColumnWidth, row2BoxHeight}, 0};
        const SectionLayout metricsSection{{rightX, row3Y - documentScrollOffset, rightColumnWidth, row3BoxHeight}, 0};

        std::vector<BuildOption> drawerOptions = buildOptionsForContext(province, drawer);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_MOUSEWHEEL)
            {
                int mouseX = 0;
                int mouseY = 0;
                SDL_GetMouseState(&mouseX, &mouseY);

                if (mouseX >= viewportWidth + kColumnGap)
                {
                    const int optionHeight = 86;
                    const int optionGap = 10;
                    const int drawerContentHeight = static_cast<int>(drawerOptions.size()) * (optionHeight + optionGap) + 12;
                    const int drawerViewportHeight = screenHeight - (kOuterPadding * 2) - 96;
                    const int maxDrawerScroll = std::max(0, drawerContentHeight - drawerViewportHeight);

                    drawer.scrollOffset -= event.wheel.y * 36;
                    drawer.scrollOffset = std::clamp(drawer.scrollOffset, 0, maxDrawerScroll);
                }
                else
                {
                    documentScrollOffset -= event.wheel.y * 48;
                    documentScrollOffset = std::clamp(documentScrollOffset, 0, maxDocumentScroll);
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                const int mouseX = event.button.x;
                const int mouseY = event.button.y;

                bool handled = false;

                for (const ClickTarget& target : clickTargets)
                {
                    if (!pointInRect(mouseX, mouseY, target.rect))
                    {
                        continue;
                    }

                    if (target.contextType == BuildContextType::direct_attachment_slot)
                    {
                        openDirectAttachmentDrawer(drawer, province, target.targetBuildingId);
                    }
                    else if (target.contextType == BuildContextType::estate_attachment_slot)
                    {
                        openEstateAttachmentDrawer(drawer, province, target.targetBuildingId);
                    }

                    handled = true;
                    break;
                }

                if (!handled && mouseX >= viewportWidth + kColumnGap)
                {
                    SDL_Rect drawerRect{viewportWidth + kColumnGap, kOuterPadding, kDrawerWidth, screenHeight - (kOuterPadding * 2)};
                    const int optionHeight = 86;
                    const int optionGap = 10;
                    int optionY = drawerRect.y + 102 - drawer.scrollOffset;

                    for (const BuildOption& option : drawerOptions)
                    {
                        SDL_Rect optionRect{drawerRect.x + 14, optionY, drawerRect.w - 28, optionHeight};
                        if (pointInRect(mouseX, mouseY, optionRect))
                        {
                            std::string resultText;
                            const bool built = applyDrawerBuildSelection(province, drawer, option, resultText);
                            drawer.subtitle = resultText;

                            if (built)
                            {
                                drawer.scrollOffset = 0;

                                if (
                                    drawer.contextType == BuildContextType::direct_attachment_slot ||
                                    drawer.contextType == BuildContextType::estate_attachment_slot
                                )
                                {
                                    if (drawer.contextType == BuildContextType::direct_attachment_slot)
                                    {
                                        openDirectAttachmentDrawer(drawer, province, drawer.targetBuildingId);
                                    }
                                    else
                                    {
                                        openEstateAttachmentDrawer(drawer, province, drawer.targetBuildingId);
                                    }
                                }
                                else
                                {
                                    openRootDrawer(drawer);
                                }
                            }

                            break;
                        }

                        optionY += optionHeight + optionGap;
                    }
                }
            }
        }

        setRenderColor(renderer, UiPalette::background);
        SDL_RenderClear(renderer);

        SDL_Rect shiftedHeader = headerRect;
        shiftedHeader.y -= documentScrollOffset;
        drawPanel(renderer, shiftedHeader);
        drawText(renderer, titleFont, "Province Building Screen", shiftedHeader.x + 16, shiftedHeader.y + 16, UiPalette::text);

        drawPanel(renderer, buildingsSection.rect);
        drawPanel(renderer, popSection.rect);
        drawPanel(renderer, infraSection.rect);
        drawPanel(renderer, resourcesSection.rect);
        drawPanel(renderer, metricsSection.rect);

        drawSectionTitle(renderer, sectionFont, buildingsSection.rect, "Buildings");
        drawSectionTitle(renderer, sectionFont, popSection.rect, "Pop Count");
        drawSectionTitle(renderer, sectionFont, infraSection.rect, "Infrastructure");
        drawSectionTitle(renderer, sectionFont, resourcesSection.rect, "Resource Totals");
        drawSectionTitle(renderer, sectionFont, metricsSection.rect, "Province Metrics");

        std::vector<ClickTarget> nextClickTargets;
        int buildingsY = buildingsSection.rect.y + 54;

        for (int rootId : province.rootBuildingIds())
        {
            const Building* root = province.getBuildingById(rootId);
            if (!root)
            {
                continue;
            }

            buildingsY += renderBuildingRootBlock(renderer, bodyFont, province, rootId, buildingsSection.rect, buildingsY, nextClickTargets);
            buildingsY += 14;
        }

        drawPopCountSection(renderer, bodyFont, popSection, province);
        drawInfrastructureSection(renderer, bodyFont, infraSection);
        drawResourcesSection(renderer, bodyFont, resourcesSection);
        drawMetricsSection(renderer, bodyFont, metricsSection);

        SDL_Rect drawerRect{
            viewportWidth + kColumnGap,
            kOuterPadding,
            kDrawerWidth,
            screenHeight - (kOuterPadding * 2)
        };
        drawPanelFilled(renderer, drawerRect, UiPalette::drawerFill, UiPalette::border);
        drawText(renderer, sectionFont, drawer.title, drawerRect.x + 14, drawerRect.y + 12, UiPalette::text);
        drawWrappedText(renderer, bodyFont, drawer.subtitle, drawerRect.x + 14, drawerRect.y + 46, drawerRect.w - 28, UiPalette::mutedText);

        if (drawerOptions.empty())
        {
            drawWrappedText(
                renderer,
                bodyFont,
                "No build options are currently available for this selection.",
                drawerRect.x + 14,
                drawerRect.y + 118,
                drawerRect.w - 28,
                UiPalette::text
            );
        }
        else
        {
            const int optionHeight = 86;
            const int optionGap = 10;
            int optionY = drawerRect.y + 102 - drawer.scrollOffset;

            for (const BuildOption& option : drawerOptions)
            {
                SDL_Rect optionRect{drawerRect.x + 14, optionY, drawerRect.w - 28, optionHeight};
                drawPanelFilled(
                    renderer,
                    optionRect,
                    option.enabled ? UiPalette::buttonFill : UiPalette::slotFill,
                    option.enabled ? UiPalette::activeBorder : UiPalette::border
                );

                drawText(renderer, sectionFont, option.label, optionRect.x + 12, optionRect.y + 10, UiPalette::text);
                drawWrappedText(
                    renderer,
                    bodyFont,
                    option.description,
                    optionRect.x + 12,
                    optionRect.y + 42,
                    optionRect.w - 24,
                    option.enabled ? UiPalette::text : UiPalette::mutedText
                );

                optionY += optionHeight + optionGap;
            }
        }

        SDL_RenderPresent(renderer);
        clickTargets = nextClickTargets;
    }
}