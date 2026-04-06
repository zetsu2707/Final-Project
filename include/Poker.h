// Header file for Poker minigame/Poker game class.
// Related Files:
// Date Created: 3/29/2026
// Last Edited: 4/5/2026

#pragma once
#include "CasinoGame.h"
#include "Player.h"
#include <string>
#include <vector>
#include <random>

class Poker {
public:
    Poker();

    void play(Player& player);

private:

    struct Card {
        std::string rank;
        std::string suit;
        int value;
    };

    class Hand {
    public:
        void addCard(const Card& card);
        int getValue() const;
        void clear();
        void show(bool hideFirstCard = false) const;
        const std::vector<Card>& getCards() const;
        int size() const;

    private:
        std::vector<Card> m_cards;
    };

    class Deck {
    public:
        Deck();

        void reset();
        void shuffle();
        Card draw();

    private:
        std::vector<Card> m_cards;
        std::mt19937 m_rng;
    };

    // Poker hand rankings
    enum class HandRank {
        HighCard,
        OnePair,
        TwoPair,
        ThreeOfAKind,
        Straight,
        Flush,
        FullHouse,
        FourOfAKind,
        StraightFlush,
        RoyalFlush
    };

    class Participant {
    public:
        virtual ~Participant() = default;

        virtual void takeTurn(Deck& deck) = 0;

        void hit(Deck& deck);         // draw a single card
        int getHandValue() const;
        void showHand(bool hideFirstCard = false) const;
        const Hand& getHand() const;
        void clearHand();

    protected:
        Hand m_hand;
    };

    class PokerPlayer : public Participant {
    public:
        void takeTurn(Deck& deck) override;
        void discardAndDraw(Deck& deck);  // for draw poker: swap cards
    };

    class AIOpponent : public Participant {
    public:
        explicit AIOpponent(std::string name);
        void takeTurn(Deck& deck) override;
        const std::string& getName() const;

    private:
        std::string m_name;
    };

    // Hand evaluation
    HandRank evaluateHand(const Hand& hand) const;
    std::string handRankToString(HandRank rank) const;
    bool winsAgainst(const Hand& a, const Hand& b) const;

    void showBanner() const;
    double getBet(const Player& player) const;
    void dealInitialHands();
    void showAllHands(bool revealOpponents = false) const;
    double bettingRound(Player& player, double& pot,
                    std::vector<bool>& foldedOpponents,
                    double currentBet);

    template <typename T>
    T getValidatedInput(T min, T max) const;

    Deck m_deck;
    PokerPlayer m_player;
    std::vector<AIOpponent> m_opponents;
};