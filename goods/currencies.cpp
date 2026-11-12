// File: goods/currencies.cpp
// Commit: Add display names for dedicated currency resources

#include "currencies.h"

const char* currencyTypeName(CurrencyType type)
{
    switch (type)
    {
        case CurrencyType::prestige:
            return "Prestige";

        case CurrencyType::piety:
            return "Piety";

        case CurrencyType::magnificence:
            return "Magnificence";

        case CurrencyType::religious_authority:
            return "Religious Authority";

        case CurrencyType::money:
            return "Money";
    }

    return "Unknown Currency";
}