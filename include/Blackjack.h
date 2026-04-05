// Header file for Blackjack minigame/Blackjack game class.
// Related Files: Blackjack.cpp, Player.h
// Date Created: 3/29/2026
// Last Edited: 4/04/2026

#pragma once
#include "CasinoGame.h"
#include "Player.h"
#include <string>
#include <vector>
#include <random>

class Blackjack {
public:
    Blackjack();

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

    class Participant {
    public:
        virtual ~Participant() = default;

        virtual void takeTurn(Deck& deck) = 0;

        void hit(Deck& deck);
        int getHandValue() const;
        void showHand(bool hideFirstCard = false) const;

    protected:
        Hand m_hand;
    };

    class BJPlayer : public Participant {
    public:
        void takeTurn(Deck& deck) override; 
    };

    class Dealer : public Participant {
    public:
        void takeTurn(Deck& deck) override;
    };

    void showBanner() const;
    double getBet(const Player& player) const;

    template <typename T>
    T getValidatedInput(T min, T max) const;
};
