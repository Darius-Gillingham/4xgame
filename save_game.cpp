// File: save_game.cpp
// Commit: Migrate save serialization from buildings and sub-buildings to holdings and buildings

#include "save_game.h"

#include <fstream>
#include <string>

namespace
{
    constexpr int kSaveVersion = 8;
    constexpr int kMinimumSupportedSaveVersion = 8;

    int terrainToInt(Terrain terrain)
    {
        return static_cast<int>(terrain);
    }

    int regionToInt(Region region)
    {
        return static_cast<int>(region);
    }

    int governmentToInt(Government government)
    {
        return static_cast<int>(government);
    }

    int rulerTraitToInt(RulerTrait trait)
    {
        return static_cast<int>(trait);
    }

    int popTypeToInt(PopType type)
    {
        return static_cast<int>(type);
    }

    int holdingTypeToInt(HoldingType type)
    {
        return static_cast<int>(type);
    }

    int buildingTypeToInt(BuildingType type)
    {
        return static_cast<int>(type);
    }

    int resourceTypeToInt(ResourceType type)
    {
        return static_cast<int>(type);
    }

    Terrain intToTerrain(int value)
    {
        if (value < static_cast<int>(Terrain::tundra) || value > static_cast<int>(Terrain::ocean))
        {
            return Terrain::ocean;
        }

        return static_cast<Terrain>(value);
    }

    Region intToRegion(int value)
    {
        if (value < static_cast<int>(Region::none) || value > static_cast<int>(Region::pacific_islands))
        {
            return Region::none;
        }

        return static_cast<Region>(value);
    }

    Government intToGovernment(int value)
    {
        if (value < static_cast<int>(Government::tribal) || value > static_cast<int>(Government::theocratic))
        {
            return Government::tribal;
        }

        return static_cast<Government>(value);
    }

    RulerTrait intToRulerTrait(int value)
    {
        if (value < static_cast<int>(RulerTrait::martial) || value > static_cast<int>(RulerTrait::administrator))
        {
            return RulerTrait::martial;
        }

        return static_cast<RulerTrait>(value);
    }

    PopType intToPopType(int value)
    {
        if (value < static_cast<int>(PopType::farmer) || value > static_cast<int>(PopType::tribesman))
        {
            return PopType::farmer;
        }

        return static_cast<PopType>(value);
    }

    HoldingType intToHoldingType(int value)
    {
        if (value < static_cast<int>(HoldingType::castle) || value > static_cast<int>(HoldingType::tribal_smallholder))
        {
            return HoldingType::tribe;
        }

        return static_cast<HoldingType>(value);
    }

    BuildingType intToBuildingType(int value)
    {
        if (value < static_cast<int>(BuildingType::explorer) || value > static_cast<int>(BuildingType::drying_racks))
        {
            return BuildingType::game_hunters;
        }

        return static_cast<BuildingType>(value);
    }

    ResourceType intToResourceType(int value)
    {
        if (value < static_cast<int>(ResourceType::prestige) || value > static_cast<int>(ResourceType::copper_tools))
        {
            return ResourceType::prestige;
        }

        return static_cast<ResourceType>(value);
    }

    GameDate dateFromTickCount(int tickCount)
    {
        GameDate date;

        if (tickCount <= 0)
        {
            return date;
        }

        date.year = 1 + (tickCount / 12);
        date.month = 1 + (tickCount % 12);

        return date;
    }

    GameDate normalizedDate(GameDate date, int fallbackTickCount)
    {
        if (date.year <= 0 || date.month <= 0 || date.month > 12)
        {
            return dateFromTickCount(fallbackTickCount);
        }

        return date;
    }

    bool readExpectedKey(std::istream& input, const std::string& expectedKey, std::string& reason)
    {
        std::string key;
        input >> key;

        if (!input || key != expectedKey)
        {
            reason = "Invalid save file. Expected key: " + expectedKey + ".";
            return false;
        }

        return true;
    }

    void writeProvinceData(std::ostream& output, const ProvinceSaveData& provinceData)
    {
        output
            << "PROVINCE "
            << provinceData.nextHoldingId << " "
            << provinceData.nextPopId << " "
            << provinceData.nextBuildingId << " "
            << provinceData.prestige << "\n";

        output << "POPS " << provinceData.pops.size() << "\n";

        for (const Pop& pop : provinceData.pops)
        {
            output
                << pop.id << " "
                << popTypeToInt(pop.type) << " "
                << pop.population << " "
                << pop.assignedHoldingId << " "
                << pop.assignedBuildingId << "\n";
        }

        output << "HOLDINGS " << provinceData.holdings.size() << "\n";

        for (const Holding& holding : provinceData.holdings)
        {
            output
                << "HOLDING "
                << holding.id << " "
                << holdingTypeToInt(holding.type) << " "
                << holding.parentId << " "
                << holding.childIds.size() << " "
                << holding.buildings.size() << "\n";

            output << "CHILDREN";

            for (int childId : holding.childIds)
            {
                output << " " << childId;
            }

            output << "\n";

            for (const Building& building : holding.buildings)
            {
                output
                    << "BUILDING "
                    << building.id << " "
                    << buildingTypeToInt(building.type) << " "
                    << building.tiers.size() << "\n";

                for (const BuildingTier& tier : building.tiers)
                {
                    output
                        << "TIER "
                        << tier.level << " "
                        << tier.count << " "
                        << tier.employedJobs << "\n";
                }
            }
        }
    }

    bool readProvinceData(std::istream& input, ProvinceSaveData& provinceData, std::string& reason)
    {
        if (!readExpectedKey(input, "PROVINCE", reason))
        {
            return false;
        }

        input
            >> provinceData.nextHoldingId
            >> provinceData.nextPopId
            >> provinceData.nextBuildingId
            >> provinceData.prestige;

        if (!input)
        {
            reason = "Invalid province metadata in save file.";
            return false;
        }

        if (!readExpectedKey(input, "POPS", reason))
        {
            return false;
        }

        size_t popCount = 0;
        input >> popCount;

        if (!input)
        {
            reason = "Invalid pop count in save file.";
            return false;
        }

        provinceData.pops.clear();
        provinceData.pops.reserve(popCount);

        for (size_t i = 0; i < popCount; ++i)
        {
            int popType = 0;

            Pop pop;
            input
                >> pop.id
                >> popType
                >> pop.population
                >> pop.assignedHoldingId
                >> pop.assignedBuildingId;

            if (!input)
            {
                reason = "Invalid pop data in save file.";
                return false;
            }

            pop.type = intToPopType(popType);
            provinceData.pops.push_back(pop);
        }

        if (!readExpectedKey(input, "HOLDINGS", reason))
        {
            return false;
        }

        size_t holdingCount = 0;
        input >> holdingCount;

        if (!input)
        {
            reason = "Invalid holding count in save file.";
            return false;
        }

        provinceData.holdings.clear();
        provinceData.holdings.reserve(holdingCount);

        for (size_t i = 0; i < holdingCount; ++i)
        {
            if (!readExpectedKey(input, "HOLDING", reason))
            {
                return false;
            }

            int holdingType = 0;
            size_t childCount = 0;
            size_t buildingCount = 0;

            Holding holding;
            input
                >> holding.id
                >> holdingType
                >> holding.parentId
                >> childCount
                >> buildingCount;

            if (!input)
            {
                reason = "Invalid holding data in save file.";
                return false;
            }

            holding.type = intToHoldingType(holdingType);

            if (!readExpectedKey(input, "CHILDREN", reason))
            {
                return false;
            }

            holding.childIds.clear();
            holding.childIds.reserve(childCount);

            for (size_t childIndex = 0; childIndex < childCount; ++childIndex)
            {
                int childId = -1;
                input >> childId;

                if (!input)
                {
                    reason = "Invalid holding child data in save file.";
                    return false;
                }

                holding.childIds.push_back(childId);
            }

            holding.buildings.clear();
            holding.buildings.reserve(buildingCount);

            for (size_t buildingIndex = 0; buildingIndex < buildingCount; ++buildingIndex)
            {
                if (!readExpectedKey(input, "BUILDING", reason))
                {
                    return false;
                }

                int buildingType = 0;
                size_t tierCount = 0;

                Building building;
                input
                    >> building.id
                    >> buildingType
                    >> tierCount;

                if (!input)
                {
                    reason = "Invalid building data in save file.";
                    return false;
                }

                building.type = intToBuildingType(buildingType);
                building.tiers.clear();
                building.tiers.reserve(tierCount);

                for (size_t tierIndex = 0; tierIndex < tierCount; ++tierIndex)
                {
                    if (!readExpectedKey(input, "TIER", reason))
                    {
                        return false;
                    }

                    BuildingTier tier;
                    input
                        >> tier.level
                        >> tier.count
                        >> tier.employedJobs;

                    if (!input)
                    {
                        reason = "Invalid building tier data in save file.";
                        return false;
                    }

                    building.tiers.push_back(tier);
                }

                holding.buildings.push_back(building);
            }

            provinceData.holdings.push_back(holding);
        }

        return true;
    }

    void writeCountryData(std::ostream& output, const std::vector<Country>& countries)
    {
        output << "COUNTRIES " << countries.size() << "\n";

        for (const Country& country : countries)
        {
            output
                << "COUNTRY "
                << country.id << " "
                << country.capitalProvinceId << " "
                << country.provinceIds.size() << " "
                << country.discoveries.size() << "\n";

            output << "COUNTRY_PROVINCES";

            for (int provinceId : country.provinceIds)
            {
                output << " " << provinceId;
            }

            output << "\n";

            for (const Discovery& discovery : country.discoveries)
            {
                output
                    << "DISCOVERY "
                    << resourceTypeToInt(discovery.resource) << " "
                    << (discovery.discovered ? 1 : 0) << " "
                    << discovery.progress << " "
                    << discovery.requiredProgress << "\n";
            }
        }
    }

    bool readCountryData(std::istream& input, std::vector<Country>& countries, std::string& reason)
    {
        if (!readExpectedKey(input, "COUNTRIES", reason))
        {
            return false;
        }

        size_t countryCount = 0;
        input >> countryCount;

        if (!input)
        {
            reason = "Invalid country count in save file.";
            return false;
        }

        countries.clear();
        countries.reserve(countryCount);

        for (size_t countryIndex = 0; countryIndex < countryCount; ++countryIndex)
        {
            if (!readExpectedKey(input, "COUNTRY", reason))
            {
                return false;
            }

            size_t provinceCount = 0;
            size_t discoveryCount = 0;

            Country country;
            input
                >> country.id
                >> country.capitalProvinceId
                >> provinceCount
                >> discoveryCount;

            if (!input)
            {
                reason = "Invalid country metadata in save file.";
                return false;
            }

            if (!readExpectedKey(input, "COUNTRY_PROVINCES", reason))
            {
                return false;
            }

            country.provinceIds.clear();
            country.provinceIds.reserve(provinceCount);

            for (size_t provinceIndex = 0; provinceIndex < provinceCount; ++provinceIndex)
            {
                int provinceId = -1;
                input >> provinceId;

                if (!input)
                {
                    reason = "Invalid country province data in save file.";
                    return false;
                }

                country.provinceIds.push_back(provinceId);
            }

            country.discoveries.clear();
            country.discoveries.reserve(discoveryCount);

            for (size_t discoveryIndex = 0; discoveryIndex < discoveryCount; ++discoveryIndex)
            {
                if (!readExpectedKey(input, "DISCOVERY", reason))
                {
                    return false;
                }

                int resource = 0;
                int discovered = 0;

                Discovery discovery;
                input
                    >> resource
                    >> discovered
                    >> discovery.progress
                    >> discovery.requiredProgress;

                if (!input)
                {
                    reason = "Invalid discovery data in save file.";
                    return false;
                }

                discovery.resource = intToResourceType(resource);
                discovery.discovered = discovered != 0;
                country.discoveries.push_back(discovery);
            }

            countries.push_back(country);
        }

        return true;
    }
}

bool saveGameToFile(
    const std::string& filePath,
    const Map& map,
    const GameState& gameState,
    std::string& reason
)
{
    std::ofstream output(filePath, std::ios::out | std::ios::trunc);

    if (!output.is_open())
    {
        reason = "Could not open save file for writing.";
        return false;
    }

    const GameDate date = normalizedDate(gameState.date, gameState.tickCount);

    output << "SAVE_VERSION " << kSaveVersion << "\n";
    output << "MAP " << map.width() << " " << map.height() << " " << map.seed() << "\n";

    output
        << "PLAYER "
        << gameState.playerCounty.countryId << " "
        << gameState.playerCounty.countyX << " "
        << gameState.playerCounty.countyY << " "
        << gameState.playerCounty.provinceId << " "
        << regionToInt(gameState.playerCounty.setup.startRegion) << " "
        << governmentToInt(gameState.playerCounty.setup.government) << " "
        << gameState.playerCounty.setup.rulerAge << " "
        << rulerTraitToInt(gameState.playerCounty.setup.primaryTrait) << " "
        << rulerTraitToInt(gameState.playerCounty.setup.secondaryTrait) << " "
        << gameState.tickCount << " "
        << date.year << " "
        << date.month << "\n";

    output
        << "RULER_NAME "
        << gameState.playerCounty.setup.rulerName.size() << " "
        << gameState.playerCounty.setup.rulerName << "\n";

    output << "TILES\n";

    for (int y = 0; y < map.height(); ++y)
    {
        for (int x = 0; x < map.width(); ++x)
        {
            const Tile& tile = map.at(x, y);

            output
                << x << " "
                << y << " "
                << (tile.land ? 1 : 0) << " "
                << terrainToInt(tile.terrain) << " "
                << regionToInt(tile.region) << " "
                << tile.polityId << " "
                << tile.provinceId << "\n";
        }
    }

    const Province* playerProvince = provinceById(
        gameState,
        gameState.playerCounty.provinceId
    );

    if (!playerProvince)
    {
        reason = "Could not find player province state for saving.";
        return false;
    }

    writeProvinceData(output, playerProvince->saveData());
    writeCountryData(output, gameState.countries);

    if (!output.good())
    {
        reason = "Save file write failed.";
        return false;
    }

    reason = "Game saved.";
    return true;
}

bool loadGameFromFile(
    const std::string& filePath,
    SavedGameData& savedGameData,
    std::string& reason
)
{
    std::ifstream input(filePath);

    if (!input.is_open())
    {
        reason = "Could not open save file.";
        return false;
    }

    savedGameData = SavedGameData{};

    if (!readExpectedKey(input, "SAVE_VERSION", reason))
    {
        return false;
    }

    int version = 0;
    input >> version;

    if (!input || version < kMinimumSupportedSaveVersion || version > kSaveVersion)
    {
        reason = "Unsupported save file.";
        return false;
    }

    if (!readExpectedKey(input, "MAP", reason))
    {
        return false;
    }

    input
        >> savedGameData.mapWidth
        >> savedGameData.mapHeight
        >> savedGameData.mapSeed;

    if (!input || savedGameData.mapWidth <= 0 || savedGameData.mapHeight <= 0)
    {
        reason = "Invalid map metadata in save file.";
        return false;
    }

    if (!readExpectedKey(input, "PLAYER", reason))
    {
        return false;
    }

    int startRegion = 0;
    int government = 0;
    int primaryTrait = 0;
    int secondaryTrait = 0;

    input
        >> savedGameData.playerCounty.countryId
        >> savedGameData.playerCounty.countyX
        >> savedGameData.playerCounty.countyY
        >> savedGameData.playerCounty.provinceId
        >> startRegion
        >> government
        >> savedGameData.playerCounty.setup.rulerAge
        >> primaryTrait
        >> secondaryTrait
        >> savedGameData.tickCount
        >> savedGameData.date.year
        >> savedGameData.date.month;

    if (!input)
    {
        reason = "Invalid player metadata in save file.";
        return false;
    }

    savedGameData.date = normalizedDate(
        savedGameData.date,
        savedGameData.tickCount
    );

    savedGameData.playerCounty.setup.startRegion = intToRegion(startRegion);
    savedGameData.playerCounty.setup.government = intToGovernment(government);
    savedGameData.playerCounty.setup.primaryTrait = intToRulerTrait(primaryTrait);
    savedGameData.playerCounty.setup.secondaryTrait = intToRulerTrait(secondaryTrait);

    if (!readExpectedKey(input, "RULER_NAME", reason))
    {
        return false;
    }

    size_t rulerNameLength = 0;
    input >> rulerNameLength;

    if (!input)
    {
        reason = "Invalid ruler name metadata in save file.";
        return false;
    }

    input.get();

    std::string rulerName;
    rulerName.resize(rulerNameLength);

    if (rulerNameLength > 0)
    {
        input.read(
            &rulerName[0],
            static_cast<std::streamsize>(rulerNameLength)
        );

        if (!input)
        {
            reason = "Invalid ruler name data in save file.";
            return false;
        }
    }

    savedGameData.playerCounty.setup.rulerName = rulerName;

    std::string trailingLine;
    std::getline(input, trailingLine);

    if (!readExpectedKey(input, "TILES", reason))
    {
        return false;
    }

    const int tileCount = savedGameData.mapWidth * savedGameData.mapHeight;

    savedGameData.tiles.clear();
    savedGameData.tiles.resize(static_cast<size_t>(tileCount));

    for (int i = 0; i < tileCount; ++i)
    {
        int x = 0;
        int y = 0;
        int land = 0;
        int terrain = 0;
        int region = 0;
        int polityId = -1;
        int provinceId = -1;

        input
            >> x
            >> y
            >> land
            >> terrain
            >> region
            >> polityId
            >> provinceId;

        if (!input)
        {
            reason = "Invalid tile data in save file.";
            return false;
        }

        if (
            x < 0 ||
            y < 0 ||
            x >= savedGameData.mapWidth ||
            y >= savedGameData.mapHeight
        )
        {
            reason = "Tile coordinate outside saved map bounds.";
            return false;
        }

        const int index = y * savedGameData.mapWidth + x;

        SavedTile tile;
        tile.land = land != 0;
        tile.terrain = intToTerrain(terrain);
        tile.region = intToRegion(region);
        tile.polityId = polityId;
        tile.provinceId = provinceId;

        savedGameData.tiles[static_cast<size_t>(index)] = tile;
    }

    if (!readProvinceData(input, savedGameData.provinceData, reason))
    {
        return false;
    }

    if (!readCountryData(input, savedGameData.countries, reason))
    {
        return false;
    }

    reason = "Game loaded.";
    return true;
}