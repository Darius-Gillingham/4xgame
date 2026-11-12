// File: window_render_discovery_text.cpp
// Commit: Move discoveries summary text into its own file

namespace WindowRender
{
    namespace
    {
        std::string discoveriesSummaryText(
            const GameState& gameState,
            int selectedProvinceId)
        {
            const Country* country = countryForProvince(gameState, selectedProvinceId);

            if (!country)
            {
                return "Discoveries\nCountry state not found.";
            }

            int discoveredCount = 0;
            int undiscoveredCount = 0;

            std::string text =
                "Discoveries\n"
                "Country: " +
                std::to_string(country->id);

            text += "\n\nDiscovered Resources:";

            for (const Discovery& discovery : country->discoveries)
            {
                if (!discovery.discovered)
                {
                    continue;
                }

                ++discoveredCount;
                text +=
                    "\n- " +
                    std::string(resourceTypeName(discovery.resource));
            }

            if (discoveredCount == 0)
            {
                text += "\nNone";
            }

            text += "\n\nUnknown Discoveries:";

            for (const Discovery& discovery : country->discoveries)
            {
                if (discovery.discovered)
                {
                    continue;
                }

                ++undiscoveredCount;
                text +=
                    "\n- Unknown Resource: " +
                    std::to_string(discovery.progress) +
                    "/" +
                    std::to_string(discovery.requiredProgress);
            }

            if (undiscoveredCount == 0)
            {
                text += "\nNone";
            }

            text +=
                "\n\nSummary:"
                "\nDiscovered: " +
                std::to_string(discoveredCount) +
                "\nUndiscovered: " +
                std::to_string(undiscoveredCount);

            return text;
        }
    }
}
