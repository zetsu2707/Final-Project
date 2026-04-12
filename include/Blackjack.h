// Header file for Blackjack minigame/Blackjack game class.
// Related Files: Blackjack.cpp, Player.h, BlackjackUI.h
// Date Created: 3/29/2026
// Last Edited: 4/12/2026

#pragma once
#include "Player.h"
#include <string>
#include <vector>
#include <random>

class Blackjack {
public:
    Blackjack();

    enum class RoundState {
        WaitingForBet,
        PlayerTurn,
        DealerTurn,
        RoundOver
    };

    enum class RoundResult {
        None,
        PlayerBlackjack,
        DealerBlackjack,
        PlayerBust,
        DealerBust,
        PlayerWin,
        DealerWin,
        Push
    };

    struct CardView {
        std::string rank;
        std::string suit;
        int value;
    };

    bool startRound(Player& player, double bet);
    void playerHit(Player& player);
    void playerStand(Player& player);
    void resetRound();

    RoundState getRoundState() const;
    RoundResult getRoundResult() const;

    std::vector<CardView> getPlayerCards() const;
    std::vector<CardView> getDealerCards() const;

    int getPlayerValue() const;
    int getDealerValue() const;
    int getDealerVisibleValue() const;

    double getCurrentBet() const;
    std::string getStatusText() const;

    bool isRoundOver() const;
    bool isDealerHoleCardHidden() const;
    bool isPlayerTurn() const;

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
        const std::vector<Card>& getCards() const;

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
        const Hand& getHand() const;
        void clearHand();

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

    void settleRound(Player& player);
    std::vector<CardView> toCardViews(const Hand& hand) const;

    Deck m_deck;
    BJPlayer m_playerHand;
    Dealer m_dealerHand;

    double m_currentBet;
    RoundState m_roundState;
    RoundResult m_roundResult;
    std::string m_statusText;
    bool m_hideDealerHoleCard;
};
