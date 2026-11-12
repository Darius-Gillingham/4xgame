// File: goods/currencies.h
// Commit: Add dedicated currency resource definitions

#pragma once

enum class CurrencyType
{
    prestige,
    piety,
    magnificence,
    religious_authority,
    money
};

const char* currencyTypeName(CurrencyType type);