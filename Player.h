#pragma once
#include <string>
#include <string_view> 

/**
 * @class Player
 * Represents the player in the casino game.
 *
 * Tracks player identity, balance, debt,
 * and statistics across all casino games.
 */

class Player {
public:
    Player(const std::string_view name, double startingBalance = 200.0);
};