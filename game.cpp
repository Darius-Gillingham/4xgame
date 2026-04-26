// File: game.cpp
// Commit: Add player setup metadata and random county spawning inside the selected starting region

#include "game.h"
#include <random>

namespace
{
    const std::vector<Region>& angloSaxonRegions()
    {
        static const std::vector<Region> regions = {Region::british_isles};
        return regions;
    }

    const std::vector<Region>& frankRegions()
    {
        static const std::vector<Region> regions = {Region::france};
        return regions;
    }

    const std::vector<Region>& frisianRegions()
    {
        static const std::vector<Region> regions = {Region::lowlands};
        return regions;
    }

    const std::vector<Region>& norseRegions()
    {
        static const std::vector<Region> regions = {Region::scandinavia};
        return regions;
    }

    const std::vector<Region>& wendRegions()
    {
        static const std::vector<Region> regions = {Region::baltic};
        return regions;
    }

    const std::vector<Region>& saxonRegions()
    {
        static const std::vector<Region> regions = {Region::north_germany};
        return regions;
    }

    const std::vector<Region>& gothRegions()
    {
        static const std::vector<Region> regions = {Region::south_germany, Region::iberia};
        return regions;
    }

    const std::vector<Region>& lombardRegions()
    {
        static const std::vector<Region> regions = {Region::italy};
        return regions;
    }

    const std::vector<Region>& bulgarRegions()
    {
        static const std::vector<Region> regions = {Region::pontic_steppe, Region::balkans};
        return regions;
    }

    const std::vector<Region>& magyarRegions()
    {
        static const std::vector<Region> regions = {Region::danubia, Region::pontic_steppe};
        return regions;
    }

    const std::vector<Region>& byzantineRegions()
    {
        static const std::vector<Region> regions = {Region::balkans, Region::anatolia};
        return regions;
    }

    const std::vector<Region>& rusRegions()
    {
        static const std::vector<Region> regions = {Region::russia};
        return regions;
    }

    const std::vector<Region>& poleRegions()
    {
        static const std::vector<Region> regions = {Region::poland};
        return regions;
    }
}

const char* cultureName(Culture culture)
{
    switch (culture)
    {
        case Culture::anglo_saxons: return "Anglo-Saxons";
        case Culture::franks: return "Franks";
        case Culture::frisians: return "Frisians";
        case Culture::norse: return "Norse";
        case Culture::wends: return "Wends";
        case Culture::saxons: return "Saxons";
        case Culture::goths: return "Goths";
        case Culture::lombards: return "Lombards";
        case Culture::bulgars: return "Bulgars";
        case Culture::magyars: return "Magyars";
        case Culture::byzantines: return "Byzantines";
        case Culture::rus: return "Rus";
        case Culture::poles: return "Poles";
        default: return "Unknown Culture";
    }
}

const char* religionName(Religion religion)
{
    switch (religion)
    {
        case Religion::catholic: return "Catholic";
        case Religion::orthodox: return "Orthodox";
        case Religion::pagan: return "Pagan";
        default: return "Unknown Religion";
    }
}

const char* governmentName(Government government)
{
    switch (government)
    {
        case Government::tribal: return "Tribal";
        case Government::aristocratic: return "Aristocratic";
        case Government::plutocratic: return "Plutocratic";
        case Government::theocratic: return "Theocratic";
        default: return "Unknown Government";
    }
}

const char* rulerTraitName(RulerTrait trait)
{
    switch (trait)
    {
        case RulerTrait::martial: return "Martial";
        case RulerTrait::builder: return "Builder";
        case RulerTrait::pious: return "Pious";
        case RulerTrait::trader: return "Trader";
        case RulerTrait::scholar: return "Scholar";
        case RulerTrait::administrator: return "Administrator";
        default: return "Unknown Trait";
    }
}

const char* regionName(Region region)
{
    switch (region)
    {
        case Region::scandinavia: return "Scandinavia";
        case Region::france: return "France";
        case Region::british_isles: return "British Isles";
        case Region::italy: return "Italy";
        case Region::south_germany: return "South Germany";
        case Region::lowlands: return "Lowlands";
        case Region::north_germany: return "North Germany";
        case Region::poland: return "Poland";
        case Region::baltic: return "Baltic";
        case Region::russia: return "Russia";
        case Region::pontic_steppe: return "Pontic Steppe";
        case Region::danubia: return "Danubia";
        case Region::balkans: return "Balkans";
        case Region::iberia: return "Iberia";
        case Region::anatolia: return "Anatolia";
        default: return "None";
    }
}

const std::vector<Region>& allowedStartingRegions(Culture culture)
{
    switch (culture)
    {
        case Culture::anglo_saxons: return angloSaxonRegions();
        case Culture::franks: return frankRegions();
        case Culture::frisians: return frisianRegions();
        case Culture::norse: return norseRegions();
        case Culture::wends: return wendRegions();
        case Culture::saxons: return saxonRegions();
        case Culture::goths: return gothRegions();
        case Culture::lombards: return lombardRegions();
        case Culture::bulgars: return bulgarRegions();
        case Culture::magyars: return magyarRegions();
        case Culture::byzantines: return byzantineRegions();
        case Culture::rus: return rusRegions();
        case Culture::poles: return poleRegions();
        default: return angloSaxonRegions();
    }
}

bool isAllowedStartingRegion(Culture culture, Region region)
{
    const std::vector<Region>& regions = allowedStartingRegions(culture);
    for (Region allowed : regions)
    {
        if (allowed == region)
        {
            return true;
        }
    }

    return false;
}

Region defaultStartingRegion(Culture culture)
{
    const std::vector<Region>& regions = allowedStartingRegions(culture);
    if (regions.empty())
    {
        return Region::british_isles;
    }

    return regions.front();
}

bool spawnPlayerCounty(Map& map, PlayerCounty& playerCounty)
{
    if (!isAllowedStartingRegion(playerCounty.setup.culture, playerCounty.setup.startRegion))
    {
        playerCounty.setup.startRegion = defaultStartingRegion(playerCounty.setup.culture);
    }

    std::vector<MapPosition> candidateTiles = map.landTilesInRegion(playerCounty.setup.startRegion);
    if (candidateTiles.empty())
    {
        return false;
    }

    std::uint32_t spawnSeed = map.seed() + 4099u;
    spawnSeed += static_cast<std::uint32_t>(playerCounty.setup.culture) * 31u;
    spawnSeed += static_cast<std::uint32_t>(playerCounty.setup.religion) * 17u;
    spawnSeed += static_cast<std::uint32_t>(playerCounty.setup.government) * 13u;
    spawnSeed += static_cast<std::uint32_t>(playerCounty.setup.rulerAge) * 7u;

    std::mt19937 rng(spawnSeed);
    std::uniform_int_distribution<int> distribution(0, static_cast<int>(candidateTiles.size()) - 1);
    const MapPosition chosen = candidateTiles[static_cast<size_t>(distribution(rng))];

    map.clearPolityOwnership();
    map.setPolityOwner(chosen.x, chosen.y, playerCounty.polityId);
    playerCounty.countyX = chosen.x;
    playerCounty.countyY = chosen.y;
    return true;
}