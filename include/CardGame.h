// Header file for CardGame abstract base class for card-based casino games.
// Sits between CasinoGame and card-based games (Blackjack, Poker).
// Provides a shared standard 52-card deck, Card struct, and shuffle logic.
// Related Files: CardGame.cpp, CasinoGame.h, Blackjack.h, Poker.h
// Date Created: 4/24/2026
// Last Edited: 4/24/2026

#pragma once

#include "CasinoGame.h"
#include <algorithm>
#include <string>
#include <vector>

class CardGame : public CasinoGame
{
public:
    struct Card
    {
        std::string rank;   // e.g. "Ace", "King", "10"
        std::string suit;   // e.g. "Hearts", "Spades"
        int         value;  // numeric value (Ace = 11, face cards = 10)
    };

    // Controls how suit/rank strings are formatted.
    // Short  : "J", "♥"  (Poker style)
    // Long   : "Jack", "Hearts"  (Blackjack style)
    enum class DeckStyle { Short, Long };

    CardGame() = default;
    virtual ~CardGame() = default;

protected:
    // Derived classes call buildDeck() in their constructor to populate m_deck
    // in whichever style they need, then use shuffle() and draw() freely.

    std::vector<Card> m_deck;

    // Populate m_deck with a fresh 52-card set in the chosen style.
    void buildDeck(DeckStyle style = DeckStyle::Long);

    // Shuffle m_deck using the inherited m_rng.
    void shuffleDeck();

    // Draw the top card; reshuffles automatically if the deck is empty.
    Card drawCard();

    // Reset and reshuffle in one call.
    void resetDeck(DeckStyle style = DeckStyle::Long);

private:
    DeckStyle m_deckStyle = DeckStyle::Long;
};
