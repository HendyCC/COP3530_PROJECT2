//preventing the file from being accidentally included twice
#pragma once
#include <string>

struct PlayerValuation {
    double marketValue;   // primary key
    int playerId;         // secondary key (breaks ties when marketValue is equal)
    std::string date;     // valuation date, e.g. "2023-06-15"

    bool operator<(const PlayerValuation& other) const {
        if (marketValue == other.marketValue) {
            return playerId < other.playerId;
        }
        return marketValue < other.marketValue;
    }

    bool operator>(const PlayerValuation& other) const {
        if (marketValue == other.marketValue) {
            return playerId > other.playerId;
        }
        return marketValue > other.marketValue;
    }

    bool operator<=(const PlayerValuation& other) const {
        if (marketValue == other.marketValue) return playerId <= other.playerId;
        return marketValue <= other.marketValue;
    }
    bool operator>=(const PlayerValuation& other) const {
        if (marketValue == other.marketValue) return playerId >= other.playerId;
        return marketValue >= other.marketValue;
    }

    bool operator==(const PlayerValuation& other) const {
        return (marketValue == other.marketValue) && (playerId == other.playerId);
    }
};
