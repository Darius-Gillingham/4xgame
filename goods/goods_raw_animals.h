// File: goods/goods_raw_animals.h
// Commit: Add raw hunted, livestock, companion, and productive animal goods

#pragma once

enum class RawAnimalGood
{
    wild_game,
    wild_fowl,
    great_beast,

    cattle,
    sheep,
    goats,
    pigs,
    horses,
    donkeys,
    camels,
    reindeer,
    water_buffalo,
    yaks,

    chickens,
    ducks,
    geese,

    llamas,
    alpacas,

    dogs,
    cats,

    bees,
    silkworms
};

const char* rawAnimalGoodName(RawAnimalGood good);