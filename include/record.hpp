//preventing the file from being accidentally included twice
#pragma once
#include <string>

struct PlayerValuation {
    double marketValue;   // primary key
    int playerId;         // secondary key (breaks ties when marketValue is equal)
    std::string date;     // valuation date, e.g. "2023-06-15"
};
