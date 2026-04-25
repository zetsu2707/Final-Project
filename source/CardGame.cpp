// Description: Implementation file for the CardGame abstract base class.
// Related Files: CardGame.h, CasinoGame.h
// Date Created: 4/24/2026
// Last Edited: 4/24/2026

#include "CardGame.h"

void CardGame::buildDeck(DeckStyle style)
{
    m_deckStyle = style;
    m_deck.clear();

    struct RankDef { std::string longName; std::string shortName; int value; };

    const std::vector<RankDef> ranks = {
        { "2",     "2",  2  }, { "3",  "3",  3  }, { "4",  "4",  4  },
        { "5",     "5",  5  }, { "6",  "6",  6  }, { "7",  "7",  7  },
        { "8",     "8",  8  }, { "9",  "9",  9  }, { "10", "10", 10 },
        { "Jack",  "J",  10 }, { "Queen", "Q", 10 }, { "King", "K", 10 },
        { "Ace",   "A",  11 }
    };

    struct SuitDef { std::string longName; std::string shortName; };

    const std::vector<SuitDef> suits = {
        { "Hearts",   "\xe2\x99\xa5" },
        { "Diamonds", "\xe2\x99\xa6" },
        { "Clubs",    "\xe2\x99\xa3" },
        { "Spades",   "\xe2\x99\xa0" }
    };

    for (const SuitDef& suit : suits)
    {
        for (const RankDef& rank : ranks)
        {
            Card card;
            card.rank  = (style == DeckStyle::Long) ? rank.longName  : rank.shortName;
            card.suit  = (style == DeckStyle::Long) ? suit.longName  : suit.shortName;
            card.value = rank.value;
            m_deck.push_back(card);
        }
    }
}

void CardGame::shuffleDeck()
{
    std::shuffle(m_deck.begin(), m_deck.end(), m_rng);
}

CardGame::Card CardGame::drawCard()
{
    if (m_deck.empty())
        resetDeck(m_deckStyle);

    Card card = m_deck.back();
    m_deck.pop_back();
    return card;
}

void CardGame::resetDeck(DeckStyle style)
{
    buildDeck(style);
    shuffleDeck();
}
