// File: window_render_colors.cpp
// Commit: Move terrain and region color selection into its own file

namespace WindowRender
{
    Color terrainColor(
        Terrain terrain)
    {
        switch (terrain)
        {
            case Terrain::tundra: return {235, 235, 225};
            case Terrain::boreal: return {24, 52, 28};
            case Terrain::woods: return {42, 78, 40};
            case Terrain::jungle: return {16, 96, 42};
            case Terrain::jungle_highlands: return {34, 112, 64};
            case Terrain::highlands: return {108, 126, 104};
            case Terrain::hills: return {140, 104, 72};
            case Terrain::mountains: return {140, 140, 140};
            case Terrain::meadow: return {126, 186, 104};
            case Terrain::plains: return {168, 186, 92};
            case Terrain::marsh: return {120, 188, 168};
            case Terrain::floodplain: return {96, 176, 120};
            case Terrain::steppe: return {210, 206, 120};
            case Terrain::savanna: return {198, 176, 92};
            case Terrain::arid: return {186, 150, 104};
            case Terrain::desert: return {226, 190, 92};
            case Terrain::oasis: return {72, 172, 132};
            case Terrain::farmlands: return {92, 210, 84};
            case Terrain::ocean: return {18, 46, 110};
            default: return {18, 46, 110};
        }
    }


    Color regionColor(
        Region region)
    {
        switch (region)
        {
            case Region::scandinavia: return {120, 160, 210};
            case Region::france: return {120, 180, 140};
            case Region::british_isles: return {210, 160, 120};
            case Region::italy: return {180, 120, 160};
            case Region::south_germany: return {170, 140, 120};
            case Region::lowlands: return {120, 210, 210};
            case Region::north_germany: return {180, 180, 120};
            case Region::poland: return {210, 180, 120};
            case Region::baltic: return {120, 140, 210};
            case Region::russia: return {150, 120, 210};
            case Region::pontic_steppe: return {210, 210, 120};
            case Region::danubia: return {120, 210, 140};
            case Region::balkans: return {210, 120, 140};
            case Region::iberia: return {210, 120, 120};
            case Region::anatolia: return {210, 150, 90};

            case Region::levant: return {214, 170, 112};
            case Region::mesopotamia: return {202, 158, 92};
            case Region::arabia: return {220, 190, 110};
            case Region::caucasus: return {130, 150, 120};
            case Region::persia: return {182, 126, 126};
            case Region::transoxiana: return {188, 170, 104};
            case Region::kazakhstan_steppe: return {194, 194, 104};
            case Region::tarim_basin: return {210, 176, 92};
            case Region::mongolia: return {176, 176, 104};
            case Region::northwest_india: return {190, 150, 92};
            case Region::gangetic_plain: return {120, 190, 120};
            case Region::deccan: return {166, 160, 92};
            case Region::bengal: return {80, 180, 120};
            case Region::sri_lanka: return {70, 170, 120};
            case Region::north_china: return {178, 172, 104};
            case Region::central_china: return {130, 184, 112};
            case Region::south_china: return {80, 166, 104};
            case Region::manchuria: return {104, 146, 126};
            case Region::korea: return {130, 170, 130};
            case Region::japan: return {150, 150, 190};
            case Region::indochina: return {70, 154, 92};
            case Region::mainland_southeast_asia: return {80, 170, 100};
            case Region::malay_peninsula: return {60, 150, 100};
            case Region::indonesia: return {50, 140, 100};
            case Region::philippines: return {70, 150, 130};

            case Region::egypt: return {214, 184, 96};
            case Region::libya: return {218, 190, 112};
            case Region::maghreb: return {194, 150, 104};
            case Region::nubia: return {194, 154, 94};
            case Region::horn_of_africa: return {182, 136, 88};
            case Region::ethiopian_highlands: return {122, 142, 94};
            case Region::swahili_coast: return {90, 160, 100};
            case Region::sahel: return {198, 174, 92};
            case Region::upper_niger: return {142, 174, 92};
            case Region::guinea_coast: return {60, 150, 80};
            case Region::congo_basin: return {34, 124, 66};
            case Region::southern_africa: return {170, 148, 92};

            case Region::arctic_america: return {210, 220, 220};
            case Region::western_canada: return {82, 132, 112};
            case Region::eastern_canada: return {92, 150, 120};
            case Region::pacific_northwest: return {56, 120, 92};
            case Region::great_plains: return {184, 184, 96};
            case Region::great_lakes: return {110, 160, 120};
            case Region::northeast_america: return {90, 150, 110};
            case Region::california: return {184, 148, 92};
            case Region::southwest_america: return {204, 156, 92};
            case Region::southeast_america: return {92, 170, 100};
            case Region::mexico: return {174, 128, 82};
            case Region::central_america: return {54, 150, 78};
            case Region::caribbean: return {72, 180, 150};
            case Region::andes: return {132, 118, 104};
            case Region::amazon: return {34, 130, 58};
            case Region::brazil: return {82, 160, 72};
            case Region::southern_cone: return {156, 174, 104};
            case Region::patagonia: return {140, 148, 130};

            case Region::australia: return {196, 146, 86};
            case Region::new_guinea: return {50, 130, 70};
            case Region::new_zealand: return {92, 150, 122};
            case Region::pacific_islands: return {68, 170, 150};

            default: return {18, 46, 110};
        }
    }

}
