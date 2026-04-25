// Roulette minigame implementation file.
// Related Files: Roulette.h, CasinoGame.h
// Date Created: 3/29/2026
// Last Edited: 4/24/2026

#include "Roulette.h"

Roulette::Roulette()
{
}

int Roulette::spinWheel()
{
    std::uniform_int_distribution<int> distribution(0, 36);
    return distribution(m_rng);
}

bool Roulette::isRedNumber(int number) const
{
    static constexpr std::array<int, 18> redNumbers =
    {
        1, 3, 5, 7, 9, 12, 14, 16, 18,
        19, 21, 23, 25, 27, 30, 32, 34, 36
    };

    return containsValue(redNumbers, number);
}

bool Roulette::isEvenMoneyBet(Roulette::BetType type) const
{
    return type == BetType::Red
        || type == BetType::Black
        || type == BetType::Even
        || type == BetType::Odd;
}

Roulette::PocketColor Roulette::getPocketColor(int number) const
{
    if (number == 0)
        return PocketColor::Green;

    return isRedNumber(number) ? PocketColor::Red : PocketColor::Black;
}

bool Roulette::isWinningBet(const BetRequest& bet, int winningNumber) const
{
    if (bet.type == BetType::Number)
        return bet.chosenNumber == winningNumber;

    if (winningNumber == 0)
        return false;

    switch (bet.type)
    {
    case BetType::Red:
        return isRedNumber(winningNumber);

    case BetType::Black:
        return !isRedNumber(winningNumber);

    case BetType::Even:
        return (winningNumber % 2) == 0;

    case BetType::Odd:
        return (winningNumber % 2) == 1;

    case BetType::Number:
        return bet.chosenNumber == winningNumber;
    }

    return false;
}

bool Roulette::canPlaceBet(const Player& player, const BetRequest& bet) const
{
    if (bet.amount <= 0.0)
        return false;

    if (!player.hasSufficientFunds(bet.amount))
        return false;

    if (bet.type == BetType::Number)
        return bet.chosenNumber >= 0 && bet.chosenNumber <= 36;

    return isEvenMoneyBet(bet.type);
}

Roulette::RoundOutcome Roulette::playRound(Player& player, const BetRequest& bet)
{
    RoundOutcome outcome{};
    outcome.betAmount = bet.amount;
    m_lastOutcomeWon = false;

    if (!canPlaceBet(player, bet))
    {
        outcome.message = "Invalid roulette bet.";
        return outcome;
    }

    if (!player.placeBet(bet.amount))
    {
        outcome.message = "Not enough balance for that bet.";
        return outcome;
    }

    outcome.success = true;
    outcome.winningNumber = spinWheel();
    outcome.winningColor = getPocketColor(outcome.winningNumber);
    outcome.win = isWinningBet(bet, outcome.winningNumber);

    if (outcome.win)
    {
        if (bet.type == BetType::Number)
        {
            outcome.payout = bet.amount * 36.0;
            outcome.netChange = bet.amount * 35.0;
            outcome.message = "Straight-up number hit.";
        }
        else
        {
            outcome.payout = bet.amount * 2.0;
            outcome.netChange = bet.amount;
            outcome.message = "Winning even-money bet.";
        }

        player.addWinnings(outcome.payout);
        m_lastOutcomeWon = true;
    }
    else
    {
        outcome.payout = 0.0;
        outcome.netChange = -bet.amount;
        outcome.message = "Bet lost.";
    }

    return outcome;
}

std::string Roulette::betTypeToString(BetType type)
{
    switch (type)
    {
    case BetType::Number: return "Number";
    case BetType::Red:    return "Red";
    case BetType::Black:  return "Black";
    case BetType::Even:   return "Even";
    case BetType::Odd:    return "Odd";
    }
    return "Unknown";
}

std::string Roulette::pocketColorToString(PocketColor color)
{
    switch (color)
    {
    case PocketColor::Green: return "Green";
    case PocketColor::Red:   return "Red";
    case PocketColor::Black: return "Black";
    }
    return "Unknown";
}
