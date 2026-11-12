// File: goods/goods_raw_plants.h
// Commit: Expand raw plant goods for regional and agricultural diversity

#pragma once

enum class RawPlantGood
{
    wheat,
    barley,
    rye,
    oats,
    millet,
    sorghum,
    rice,
    corn,

    fodder,
    
    beans,
    peas,
    lentils,
    chickpeas,
    soybeans,
    peanuts,

    potatoes,
    sweet_potatoes,
    cassava,
    yams,
    turnips,

    onions,
    chilies,
    squash,

    dill,
    mint,

    apples,
    pears,
    plums,
    peaches,
    apricots,
    figs,
    pomegranites,
    dates,
    citrus,
    grapes,
    bananas,
    plantains,
    pineapples,
    avocados,

    nuts,
    pistachios,
    acorns,
    walnuts,
    chestnuts,
    almonds,
    hazelnuts,

    olives,
    sesame,
    mustard,
    rapeseed,
    sunflower,
    safflower,

    sugarcane,
    cocoa,

    cotton,
    flax,
    flaxseed,
    hemp,

    madder,
    indigo,

    incense,
    poppies,
    nutmeg,
    black_pepper,
    qat,
    cannabis,
    tobacco,
    mushrooms,
    psychedelics,
    tea,
    cloves,
    scented_oils,
    lavender,
    vanilla,

    raw_rubber,
    wax
};

const char* rawPlantGoodName(RawPlantGood good);