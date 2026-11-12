// File: game.cpp
// Commit: Synchronize raised building levies with persistent units at province centers

#include "game.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

namespace
{
    constexpr int kDefaultDiscoveryRequiredProgress = 1000;
    constexpr int kExplorerDiscoveryProgressPerWorker = 10;

    int tileDistanceScore(const MapPosition& left, const MapPosition& right)
    {
        const int dx = left.x - right.x;
        const int dy = left.y - right.y;

        return (dx * dx) + (dy * dy);
    }

    MapPosition findRegionAnchor(const Map& map, Region region)
    {
        for (int y = 0; y < map.height(); ++y)
        {
            for (int x = 0; x < map.width(); ++x)
            {
                const Tile& tile = map.at(x, y);

                if (tile.region == region)
                {
                    return MapPosition{x, y};
                }
            }
        }

        return MapPosition{
            map.width() / 2,
            map.height() / 2
        };
    }

    std::vector<MapPosition> collectAllLandTiles(const Map& map)
    {
        std::vector<MapPosition> positions;

        for (int y = 0; y < map.height(); ++y)
        {
            for (int x = 0; x < map.width(); ++x)
            {
                const Tile& tile = map.at(x, y);

                if (tile.land)
                {
                    positions.push_back(
                        MapPosition{x, y}
                    );
                }
            }
        }

        return positions;
    }

    std::vector<MapPosition> closestLandTilesToRegion(
        const Map& map,
        Region region
    )
    {
        const std::vector<MapPosition> allLandTiles =
            collectAllLandTiles(map);

        std::vector<MapPosition> closestTiles;

        if (allLandTiles.empty())
        {
            return closestTiles;
        }

        const MapPosition anchor =
            findRegionAnchor(map, region);

        int bestScore =
            std::numeric_limits<int>::max();

        for (const MapPosition& position : allLandTiles)
        {
            const int score =
                tileDistanceScore(
                    position,
                    anchor
                );

            if (score < bestScore)
            {
                bestScore = score;
                closestTiles.clear();
                closestTiles.push_back(position);
            }
            else if (score == bestScore)
            {
                closestTiles.push_back(position);
            }
        }

        return closestTiles;
    }

    bool containsProvinceId(
        const std::vector<int>& provinceIds,
        int provinceId
    )
    {
        for (int existingProvinceId : provinceIds)
        {
            if (existingProvinceId == provinceId)
            {
                return true;
            }
        }

        return false;
    }

    std::vector<int> collectLandProvinceIds(const Map& map)
    {
        std::vector<int> provinceIds;

        for (int y = 0; y < map.height(); ++y)
        {
            for (int x = 0; x < map.width(); ++x)
            {
                const Tile& tile = map.at(x, y);

                if (
                    !tile.land ||
                    tile.provinceId < 0
                )
                {
                    continue;
                }

                if (
                    !containsProvinceId(
                        provinceIds,
                        tile.provinceId
                    )
                )
                {
                    provinceIds.push_back(
                        tile.provinceId
                    );
                }
            }
        }

        return provinceIds;
    }

    void setCountryOwnerForProvince(
        Map& map,
        int provinceId,
        int countryId
    )
    {
        if (provinceId < 0)
        {
            return;
        }

        for (int y = 0; y < map.height(); ++y)
        {
            for (int x = 0; x < map.width(); ++x)
            {
                const Tile& current =
                    map.at(x, y);

                if (
                    !current.land ||
                    current.provinceId != provinceId
                )
                {
                    continue;
                }

                map.setPolityOwner(
                    x,
                    y,
                    countryId
                );
            }
        }
    }

    MapPosition provinceTileCenter(
        const Map& map,
        int provinceId
    )
    {
        std::vector<MapPosition> provinceTiles;

        long long totalX = 0;
        long long totalY = 0;

        for (int y = 0; y < map.height(); ++y)
        {
            for (int x = 0; x < map.width(); ++x)
            {
                const Tile& tile =
                    map.at(x, y);

                if (
                    !tile.land ||
                    tile.provinceId != provinceId
                )
                {
                    continue;
                }

                provinceTiles.push_back(
                    MapPosition{x, y}
                );

                totalX += x;
                totalY += y;
            }
        }

        if (provinceTiles.empty())
        {
            return MapPosition{-1, -1};
        }

        const MapPosition averagePosition{
            static_cast<int>(
                totalX /
                static_cast<long long>(
                    provinceTiles.size()
                )
            ),
            static_cast<int>(
                totalY /
                static_cast<long long>(
                    provinceTiles.size()
                )
            )
        };

        MapPosition centerPosition =
            provinceTiles.front();

        int bestDistance =
            tileDistanceScore(
                centerPosition,
                averagePosition
            );

        for (const MapPosition& position : provinceTiles)
        {
            const int distance =
                tileDistanceScore(
                    position,
                    averagePosition
                );

            if (distance < bestDistance)
            {
                bestDistance = distance;
                centerPosition = position;
            }
        }

        return centerPosition;
    }

    bool unitMatchesLevy(
        const Unit& unit,
        int provinceId,
        int holdingId,
        int buildingId,
        int levySlotIndex
    )
    {
        return
            unit.sourceProvinceId == provinceId &&
            unit.sourceHoldingId == holdingId &&
            unit.sourceBuildingId == buildingId &&
            unit.sourceLevySlotIndex == levySlotIndex;
    }

    const Unit* findUnitForLevy(
        const GameState& gameState,
        int provinceId,
        int holdingId,
        int buildingId,
        int levySlotIndex
    )
    {
        for (const Unit& unit : gameState.units)
        {
            if (
                unitMatchesLevy(
                    unit,
                    provinceId,
                    holdingId,
                    buildingId,
                    levySlotIndex
                )
            )
            {
                return &unit;
            }
        }

        return nullptr;
    }

    bool sourceLevyIsRaised(
        const GameState& gameState,
        const Unit& unit
    )
    {
        const Province* province =
            provinceById(
                gameState,
                unit.sourceProvinceId
            );

        if (!province)
        {
            return false;
        }

        const Holding* holding =
            province->getHoldingById(
                unit.sourceHoldingId
            );

        if (!holding)
        {
            return false;
        }

        for (const Building& building : holding->buildings)
        {
            if (
                building.id !=
                unit.sourceBuildingId
            )
            {
                continue;
            }

            for (const Levy& levy : building.levies)
            {
                if (
                    levy.slotIndex ==
                        unit.sourceLevySlotIndex &&
                    levy.raised
                )
                {
                    return true;
                }
            }

            return false;
        }

        return false;
    }

    void addDiscovery(
        Country& country,
        ResourceType resource,
        bool discovered
    )
    {
        Discovery discovery;
        discovery.resource = resource;
        discovery.discovered = discovered;
        discovery.progress =
            discovered
                ? kDefaultDiscoveryRequiredProgress
                : 0;

        discovery.requiredProgress =
            kDefaultDiscoveryRequiredProgress;

        country.discoveries.push_back(discovery);
    }

    void initializeCountryDiscoveries(Country& country)
    {
        country.discoveries.clear();

        addDiscovery(country, ResourceType::wild_game, true);
        addDiscovery(country, ResourceType::wild_fowl, true);
        addDiscovery(country, ResourceType::meat, true);
        addDiscovery(country, ResourceType::fish, false);
        addDiscovery(country, ResourceType::shellfish, false);
        addDiscovery(country, ResourceType::wheat, false);
        addDiscovery(country, ResourceType::barley, false);
        addDiscovery(country, ResourceType::fodder, false);
        addDiscovery(country, ResourceType::onion, false);
        addDiscovery(country, ResourceType::turnip, false);
        addDiscovery(country, ResourceType::apples, false);
        addDiscovery(country, ResourceType::dill, false);
        addDiscovery(country, ResourceType::wood, false);
        addDiscovery(country, ResourceType::timber, false);
        addDiscovery(country, ResourceType::flint, false);
        addDiscovery(country, ResourceType::basalt, false);
        addDiscovery(country, ResourceType::bone, false);
        addDiscovery(country, ResourceType::antler, false);
        addDiscovery(country, ResourceType::horn, false);
        addDiscovery(country, ResourceType::hide, false);
        addDiscovery(country, ResourceType::leather, false);
        addDiscovery(country, ResourceType::lard, false);
        addDiscovery(country, ResourceType::feathers, false);
        addDiscovery(country, ResourceType::raw_clay, false);
        addDiscovery(country, ResourceType::pottery, false);
        addDiscovery(country, ResourceType::charcoal, false);
        addDiscovery(country, ResourceType::thatch, false);
        addDiscovery(country, ResourceType::rope, false);
        addDiscovery(country, ResourceType::deer, false);
        addDiscovery(country, ResourceType::boar, false);
        addDiscovery(country, ResourceType::hare, false);
        addDiscovery(country, ResourceType::beaver, false);
        addDiscovery(country, ResourceType::seal, false);
        addDiscovery(country, ResourceType::whale, false);
        addDiscovery(country, ResourceType::salmon, false);
        addDiscovery(country, ResourceType::river_fish, false);
        addDiscovery(country, ResourceType::lake_fish, false);
        addDiscovery(country, ResourceType::eel, false);
        addDiscovery(country, ResourceType::crab, false);
        addDiscovery(country, ResourceType::eggs, false);
        addDiscovery(country, ResourceType::honey, false);
        addDiscovery(country, ResourceType::berries, false);
        addDiscovery(country, ResourceType::nuts, false);
        addDiscovery(country, ResourceType::roots, false);
        addDiscovery(country, ResourceType::mushrooms, false);
        addDiscovery(country, ResourceType::herbs, false);
        addDiscovery(country, ResourceType::reeds, false);
        addDiscovery(country, ResourceType::plant_fiber, false);
        addDiscovery(country, ResourceType::resin, false);
        addDiscovery(country, ResourceType::bark, false);
        addDiscovery(country, ResourceType::sand, false);
        addDiscovery(country, ResourceType::limestone, false);
        addDiscovery(country, ResourceType::granite, false);
        addDiscovery(country, ResourceType::obsidian, false);
        addDiscovery(country, ResourceType::native_copper, false);
        addDiscovery(country, ResourceType::bog_iron, false);
        addDiscovery(country, ResourceType::salt, false);
        addDiscovery(country, ResourceType::peat, false);
        addDiscovery(country, ResourceType::cured_meat, false);
        addDiscovery(country, ResourceType::smoked_fish, false);
        addDiscovery(country, ResourceType::dried_fish, false);
        addDiscovery(country, ResourceType::dried_berries, false);
        addDiscovery(country, ResourceType::baskets, false);
        addDiscovery(country, ResourceType::mats, false);
        addDiscovery(country, ResourceType::nets, false);
        addDiscovery(country, ResourceType::worked_bone, false);
        addDiscovery(country, ResourceType::worked_antler, false);
        addDiscovery(country, ResourceType::worked_horn, false);
        addDiscovery(country, ResourceType::worked_shell, false);
        addDiscovery(country, ResourceType::worked_stone, false);
        addDiscovery(country, ResourceType::copper_tools, false);
    }

    int employedExplorerJobsForProvince(
        const Province& province
    )
    {
        int total = 0;

        for (const Holding& holding : province.holdings())
        {
            for (const Building& building : holding.buildings)
            {
                if (
                    building.type !=
                    BuildingType::explorer
                )
                {
                    continue;
                }

                for (
                    const BuildingTier& tier :
                    building.tiers
                )
                {
                    total += tier.employedJobs;
                }
            }
        }

        return total;
    }

    int employedExplorerJobsForCountry(
        const GameState& gameState,
        const Country& country
    )
    {
        int total = 0;

        for (int provinceId : country.provinceIds)
        {
            const Province* province =
                provinceById(
                    gameState,
                    provinceId
                );

            if (!province)
            {
                continue;
            }

            total +=
                employedExplorerJobsForProvince(
                    *province
                );
        }

        return total;
    }

    void advanceCountryDiscovery(
        Country& country,
        int explorerJobs
    )
    {
        if (explorerJobs <= 0)
        {
            return;
        }

        const int progressToAdd =
            explorerJobs *
            kExplorerDiscoveryProgressPerWorker;

        for (Discovery& discovery : country.discoveries)
        {
            if (discovery.discovered)
            {
                continue;
            }

            discovery.progress += progressToAdd;

            if (
                discovery.progress >=
                discovery.requiredProgress
            )
            {
                discovery.progress =
                    discovery.requiredProgress;

                discovery.discovered = true;
            }

            return;
        }
    }

    void initializeProvinceCountries(
        Map& map,
        GameState& gameState
    )
    {
        map.clearPolityOwnership();

        gameState.provinceSlots.clear();
        gameState.countries.clear();
        gameState.units.clear();
        gameState.nextUnitId = 1;

        const std::vector<int> provinceIds =
            collectLandProvinceIds(map);

        int nextCountryId = 1;

        for (int provinceId : provinceIds)
        {
            int countryId = nextCountryId;

            if (
                provinceId ==
                gameState.playerCounty.provinceId
            )
            {
                countryId =
                    gameState.playerCounty.countryId;
            }
            else
            {
                ++nextCountryId;
            }

            setCountryOwnerForProvince(
                map,
                provinceId,
                countryId
            );

            ProvinceSlot provinceSlot;
            provinceSlot.provinceId = provinceId;
            provinceSlot.countryId = countryId;

            gameState.provinceSlots.push_back(
                provinceSlot
            );

            Country country;
            country.id = countryId;
            country.capitalProvinceId = provinceId;
            country.provinceIds.push_back(provinceId);

            initializeCountryDiscoveries(country);

            gameState.countries.push_back(country);
        }
    }
}

const char* governmentName(Government government)
{
    switch (government)
    {
        case Government::tribal:
            return "Tribal";

        case Government::aristocratic:
            return "Aristocratic";

        case Government::plutocratic:
            return "Plutocratic";

        case Government::theocratic:
            return "Theocratic";

        default:
            return "Unknown Government";
    }
}

const char* rulerTraitName(RulerTrait trait)
{
    switch (trait)
    {
        case RulerTrait::martial:
            return "Martial";

        case RulerTrait::builder:
            return "Builder";

        case RulerTrait::pious:
            return "Pious";

        case RulerTrait::trader:
            return "Trader";

        case RulerTrait::scholar:
            return "Scholar";

        case RulerTrait::administrator:
            return "Administrator";

        default:
            return "Unknown Trait";
    }
}

const char* regionName(Region region)
{
    switch (region)
    {
        case Region::scandinavia:
            return "Scandinavia";

        case Region::france:
            return "France";

        case Region::british_isles:
            return "British Isles";

        case Region::italy:
            return "Italy";

        case Region::south_germany:
            return "South Germany";

        case Region::lowlands:
            return "Lowlands";

        case Region::north_germany:
            return "North Germany";

        case Region::poland:
            return "Poland";

        case Region::baltic:
            return "Baltic";

        case Region::russia:
            return "Russia";

        case Region::pontic_steppe:
            return "Pontic Steppe";

        case Region::danubia:
            return "Danubia";

        case Region::balkans:
            return "Balkans";

        case Region::iberia:
            return "Iberia";

        case Region::anatolia:
            return "Anatolia";

        case Region::levant:
            return "Levant";

        case Region::mesopotamia:
            return "Mesopotamia";

        case Region::arabia:
            return "Arabia";

        case Region::caucasus:
            return "Caucasus";

        case Region::persia:
            return "Persia";

        case Region::transoxiana:
            return "Transoxiana";

        case Region::kazakhstan_steppe:
            return "Kazakhstan Steppe";

        case Region::tarim_basin:
            return "Tarim Basin";

        case Region::mongolia:
            return "Mongolia";

        case Region::northwest_india:
            return "Northwest India";

        case Region::gangetic_plain:
            return "Gangetic Plain";

        case Region::deccan:
            return "Deccan";

        case Region::bengal:
            return "Bengal";

        case Region::sri_lanka:
            return "Sri Lanka";

        case Region::north_china:
            return "North China";

        case Region::central_china:
            return "Central China";

        case Region::south_china:
            return "South China";

        case Region::manchuria:
            return "Manchuria";

        case Region::korea:
            return "Korea";

        case Region::japan:
            return "Japan";

        case Region::indochina:
            return "Indochina";

        case Region::mainland_southeast_asia:
            return "Mainland Southeast Asia";

        case Region::malay_peninsula:
            return "Malay Peninsula";

        case Region::indonesia:
            return "Indonesia";

        case Region::philippines:
            return "Philippines";

        case Region::egypt:
            return "Egypt";

        case Region::libya:
            return "Libya";

        case Region::maghreb:
            return "Maghreb";

        case Region::nubia:
            return "Nubia";

        case Region::horn_of_africa:
            return "Horn of Africa";

        case Region::ethiopian_highlands:
            return "Ethiopian Highlands";

        case Region::swahili_coast:
            return "Swahili Coast";

        case Region::sahel:
            return "Sahel";

        case Region::upper_niger:
            return "Upper Niger";

        case Region::guinea_coast:
            return "Guinea Coast";

        case Region::congo_basin:
            return "Congo Basin";

        case Region::southern_africa:
            return "Southern Africa";

        default:
            return "None";
    }
}

const char* monthName(int month)
{
    switch (month)
    {
        case 1:
            return "January";

        case 2:
            return "February";

        case 3:
            return "March";

        case 4:
            return "April";

        case 5:
            return "May";

        case 6:
            return "June";

        case 7:
            return "July";

        case 8:
            return "August";

        case 9:
            return "September";

        case 10:
            return "October";

        case 11:
            return "November";

        case 12:
            return "December";

        default:
            return "Unknown Month";
    }
}

ProvinceSlot* provinceSlotById(
    GameState& gameState,
    int provinceId
)
{
    for (ProvinceSlot& provinceSlot : gameState.provinceSlots)
    {
        if (provinceSlot.provinceId == provinceId)
        {
            return &provinceSlot;
        }
    }

    return nullptr;
}

const ProvinceSlot* provinceSlotById(
    const GameState& gameState,
    int provinceId
)
{
    for (
        const ProvinceSlot& provinceSlot :
        gameState.provinceSlots
    )
    {
        if (provinceSlot.provinceId == provinceId)
        {
            return &provinceSlot;
        }
    }

    return nullptr;
}

Province* provinceById(
    GameState& gameState,
    int provinceId
)
{
    ProvinceSlot* provinceSlot =
        provinceSlotById(
            gameState,
            provinceId
        );

    if (!provinceSlot)
    {
        return nullptr;
    }

    return &provinceSlot->province;
}

const Province* provinceById(
    const GameState& gameState,
    int provinceId
)
{
    const ProvinceSlot* provinceSlot =
        provinceSlotById(
            gameState,
            provinceId
        );

    if (!provinceSlot)
    {
        return nullptr;
    }

    return &provinceSlot->province;
}

Country* countryById(
    GameState& gameState,
    int countryId
)
{
    for (Country& country : gameState.countries)
    {
        if (country.id == countryId)
        {
            return &country;
        }
    }

    return nullptr;
}

const Country* countryById(
    const GameState& gameState,
    int countryId
)
{
    for (const Country& country : gameState.countries)
    {
        if (country.id == countryId)
        {
            return &country;
        }
    }

    return nullptr;
}

bool spawnPlayerCounty(
    Map& map,
    PlayerCounty& playerCounty
)
{
    std::vector<MapPosition> candidateTiles =
        map.landTilesInRegion(
            playerCounty.setup.startRegion
        );

    if (candidateTiles.empty())
    {
        candidateTiles =
            closestLandTilesToRegion(
                map,
                playerCounty.setup.startRegion
            );
    }

    if (candidateTiles.empty())
    {
        return false;
    }

    std::uint32_t spawnSeed =
        map.seed() + 4099u;

    spawnSeed +=
        static_cast<std::uint32_t>(
            playerCounty.setup.government
        ) *
        13u;

    spawnSeed +=
        static_cast<std::uint32_t>(
            playerCounty.setup.rulerAge
        ) *
        7u;

    std::mt19937 rng(spawnSeed);

    std::uniform_int_distribution<int> distribution(
        0,
        static_cast<int>(
            candidateTiles.size()
        ) -
        1
    );

    const MapPosition chosen =
        candidateTiles[
            static_cast<std::size_t>(
                distribution(rng)
            )
        ];

    const int chosenProvinceId =
        map.at(
            chosen.x,
            chosen.y
        ).provinceId;

    if (chosenProvinceId < 0)
    {
        return false;
    }

    playerCounty.countyX = chosen.x;
    playerCounty.countyY = chosen.y;
    playerCounty.provinceId = chosenProvinceId;

    return true;
}

bool initializeGameState(
    Map& map,
    const PlayerSetup& setup,
    GameState& gameState
)
{
    gameState = GameState{};
    gameState.playerCounty.setup = setup;
    gameState.playerCounty.countryId = 0;
    gameState.playerCounty.provinceId = -1;
    gameState.nextUnitId = 1;
    gameState.tickCount = 0;
    gameState.date.year = 1;
    gameState.date.month = 1;

    if (
        !spawnPlayerCounty(
            map,
            gameState.playerCounty
        )
    )
    {
        return false;
    }

    initializeProvinceCountries(
        map,
        gameState
    );

    return true;
}

void synchronizeRaisedLevyUnits(
    const Map& map,
    GameState& gameState
)
{
    gameState.units.erase(
        std::remove_if(
            gameState.units.begin(),
            gameState.units.end(),
            [&gameState](const Unit& unit)
            {
                return
                    !sourceLevyIsRaised(
                        gameState,
                        unit
                    );
            }
        ),
        gameState.units.end()
    );

    for (
        const ProvinceSlot& provinceSlot :
        gameState.provinceSlots
    )
    {
        const MapPosition spawnPosition =
            provinceTileCenter(
                map,
                provinceSlot.provinceId
            );

        if (
            spawnPosition.x < 0 ||
            spawnPosition.y < 0
        )
        {
            continue;
        }

        for (
            const Holding& holding :
            provinceSlot.province.holdings()
        )
        {
            for (
                const Building& building :
                holding.buildings
            )
            {
                for (const Levy& levy : building.levies)
                {
                    if (!levy.raised)
                    {
                        continue;
                    }

                    if (
                        findUnitForLevy(
                            gameState,
                            provinceSlot.provinceId,
                            holding.id,
                            building.id,
                            levy.slotIndex
                        )
                    )
                    {
                        continue;
                    }

                    Unit unit;
                    unit.id = gameState.nextUnitId++;
                    unit.countryId =
                        provinceSlot.countryId;

                    unit.sourceProvinceId =
                        provinceSlot.provinceId;

                    unit.sourceHoldingId =
                        holding.id;

                    unit.sourceBuildingId =
                        building.id;

                    unit.sourceLevySlotIndex =
                        levy.slotIndex;

                    unit.position =
                        spawnPosition;

                    unit.destination =
                        spawnPosition;

                    unit.moving = false;
                    unit.strength = 1;

                    gameState.units.push_back(unit);
                }
            }
        }
    }
}

void advanceGameMonth(GameState& gameState)
{
    ++gameState.date.month;

    if (gameState.date.month > 12)
    {
        gameState.date.month = 1;
        ++gameState.date.year;
    }

    ++gameState.tickCount;
}

void tickGame(GameState& gameState)
{
    for (
        ProvinceSlot& provinceSlot :
        gameState.provinceSlots
    )
    {
        ProvinceResourceSummary summary =
            provinceSlot.province.resourceSummary();

        for (
            const ResourceAmount& amount :
            summary.net
        )
        {
            if (
                amount.type !=
                    ResourceType::prestige ||
                amount.amount == 0
            )
            {
                continue;
            }

            if (amount.amount > 0)
            {
                provinceSlot.province.addPrestige(
                    amount.amount
                );
            }
            else
            {
                provinceSlot.province.spendPrestige(
                    -amount.amount
                );
            }
        }
    }

    for (Country& country : gameState.countries)
    {
        const int explorerJobs =
            employedExplorerJobsForCountry(
                gameState,
                country
            );

        advanceCountryDiscovery(
            country,
            explorerJobs
        );
    }

    advanceGameMonth(gameState);
}