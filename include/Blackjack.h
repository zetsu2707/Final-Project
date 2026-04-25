// Header file for Blackjack minigame/Blackjack game class.
// Related Files: Blackjack.cpp, CardGame.h, BlackjackUI.h
// Date Created: 3/29/2026
// Last Edited: 4/24/2026

#pragma once
#include "CardGame.h"
#include <string>
#include <vector>

class Blackjack : public CardGame {
public:
    Blackjack();

    std::string getName() const override { return "Blackjack"; }
    bool didPlayerWin() const override;

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
    class Hand {
    public:
        void addCard(const CardGame::Card& card);
        int  getValue() const;
        void clear();
        const std::vector<CardGame::Card>& getCards() const;
    private:
        std::vector<CardGame::Card> m_cards;
    };

    class Participant {
    public:
        virtual ~Participant() = default;
        virtual void takeTurn(Blackjack& game) = 0;

        void hit(Blackjack& game);
        int getHandValue() const;
        const Hand& getHand() const;
        void clearHand();
    protected:
        Hand m_hand;
    };

    class BJPlayer : public Participant {
    public:
        void takeTurn(Blackjack& game) override;
    };

    class Dealer : public Participant {
    public:
        void takeTurn(Blackjack& game) override;
    };

    void settleRound(Player& player);
    std::vector<CardView> toCardViews(const Hand& hand) const;

    BJPlayer m_playerHand;
    Dealer   m_dealerHand;

    double m_currentBet;
    RoundState m_roundState;
    RoundResult m_roundResult;
    std::string m_statusText;
    bool m_hideDealerHoleCard;
};
