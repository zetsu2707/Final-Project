// Header file for Roulette minigame/Roulette game class.
// Related Files: Roulette.cpp, CasinoGame.h
// Date Created: 3/29/2026
// Last Edited: 4/24/2026

#pragma once

#include "CasinoGame.h"

#include <array>
#include <string>

class Roulette : public CasinoGame
{
public:
    enum class BetType
    {
        Number,
        Red,
        Black,
        Even,
        Odd
    };

    enum class PocketColor
    {
        Green,
        Red,
        Black
    };

    struct BetRequest
    {
        BetType type = BetType::Red;
        int chosenNumber = -1;
        double amount = 0.0;
    };

    struct RoundOutcome
    {
        bool success = false;
        bool win = false;

        int winningNumber = 0;
        PocketColor winningColor = PocketColor::Green;

        double betAmount = 0.0;
        double payout = 0.0;
        double netChange = 0.0;

        std::string message;
    };

    Roulette();

    std::string getName() const override { return "Roulette"; }
    bool didPlayerWin() const override { return m_lastOutcomeWon; }

    bool canPlaceBet(const Player& player, const BetRequest& bet) const;
    RoundOutcome playRound(Player& player, const BetRequest& bet);

    static std::string betTypeToString(BetType type);
    static std::string pocketColorToString(PocketColor color);

    template <typename Container, typename ValueType>
    static bool containsValue(const Container& container, const ValueType& value)
    {
        for (const auto& item : container)
        {
            if (item == value)
                return true;
        }
        return false;
    }

private:
    int spinWheel();
    bool isRedNumber(int number) const;
    bool isEvenMoneyBet(BetType type) const;
    PocketColor getPocketColor(int number) const;
    bool isWinningBet(const BetRequest& bet, int winningNumber) const;

    bool m_lastOutcomeWon = false;
};
