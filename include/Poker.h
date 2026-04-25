// Header file for Poker minigame/Poker game class.
// Related Files: CardGame.h, Poker.cpp, PokerUI.h
// Date Created: 3/29/2026
// Last Edited: 4/24/2026

#pragma once

#include "CardGame.h"

#include <string>
#include <vector>

class Poker : public CardGame
{
public:
    struct CardView
    {
        std::string rank;
        std::string suit;
        int value;
    };

    struct OpponentView
    {
        std::string name;
        std::vector<CardView> cards;
        bool folded;
        double contribution;
        std::string bestHandText;
    };

    enum class RoundState
    {
        WaitingForBet,
        PlayerTurn,
        Showdown,
        RoundOver
    };

    enum class Phase
    {
        None,
        PreFlop,
        Flop,
        Turn,
        River,
        Showdown
    };

    enum class RoundResult
    {
        None,
        PlayerFolded,
        PlayerWin,
        OpponentWin,
        SplitPot
    };

    Poker();

    std::string getName() const override { return "Poker"; }
    bool didPlayerWin() const override;

    void resetRound();
    bool startRound(Player& player, double ante);

    void playerFold(Player& player);
    void playerCheckCall(Player& player);
    void playerRaise(Player& player, double raiseAmount);

    RoundState getRoundState() const;
    Phase getPhase() const;
    RoundResult getRoundResult() const;

    std::vector<CardView> getPlayerCards() const;
    std::vector<CardView> getCommunityCards() const;
    std::vector<OpponentView> getOpponents() const;

    double getAnte() const;
    double getPot() const;
    double getCurrentBet() const;
    double getAmountToCall() const;
    double getPlayerContribution() const;

    std::string getStatusText() const;
    std::string getPhaseText() const;
    std::string getPlayerBestHandText() const;
    std::string getWinningSummaryText() const;

    bool isRoundOver() const;
    bool areOpponentHoleCardsHidden() const;
    bool isPlayerTurn() const;

private:
    class Hand
    {
    public:
        void addCard(const CardGame::Card& card);
        void clear();
        const std::vector<CardGame::Card>& getCards() const;

    private:
        std::vector<CardGame::Card> m_cards;
    };

    enum class HandRank
    {
        HighCard = 0,
        OnePair,
        TwoPair,
        ThreeOfAKind,
        Straight,
        Flush,
        FullHouse,
        FourOfAKind,
        StraightFlush
    };

    struct EvaluatedHand
    {
        HandRank rank;
        std::vector<int> tieBreakers;
    };

    struct Opponent
    {
        std::string name;
        Hand hand;
        bool folded;
        double contribution;
        double totalCommitted;
        std::string bestHandText;
    };

    void buildPokerDeck();
    void dealHoleCards();

    void beginPhase(Phase phase);
    void dealFlop();
    void dealTurn();
    void dealRiver();
    void advanceToNextPhase(Player& player, const std::string& prefixStatus);

    void resolveOpponentsAfterPlayerCheck(Player& player);
    void resolveOpponentsFacingPlayerBet(Player& player);
    void maybeAwardPotIfEveryoneFolded(Player& player);

    std::vector<CardGame::Card> buildCombinedCards(const Hand& holeCards) const;

    EvaluatedHand evaluateBestHand(const std::vector<CardGame::Card>& cards) const;
    EvaluatedHand evaluateFiveCardHand(const std::vector<CardGame::Card>& cards) const;
    static bool isBetterHand(const EvaluatedHand& left, const EvaluatedHand& right);
    static bool isEqualHand(const EvaluatedHand& left, const EvaluatedHand& right);
    static std::string handRankToString(HandRank rank);
    static std::string phaseToString(Phase phase);

    int estimateOpponentStrength(const Opponent& opponent) const;
    bool shouldOpponentLeadBet(const Opponent& opponent) const;
    bool shouldOpponentFoldToBet(const Opponent& opponent, double amountToCall) const;
    double chooseOpponentBetSize() const;
    int randomPercent();

    void settlePlayerFold();
    void settlePlayerWin(Player& player, const std::string& statusText);
    void settleShowdown(Player& player);

    std::vector<CardView> toCardViews(const std::vector<CardGame::Card>& cards) const;

    Hand m_playerHand;
    std::vector<Opponent> m_opponents;
    std::vector<CardGame::Card> m_communityCards;

    double m_ante;
    double m_pot;
    double m_currentBet;
    double m_playerContribution;
    double m_playerTotalCommitted;

    RoundState m_roundState;
    Phase m_phase;
    RoundResult m_roundResult;

    bool m_hideOpponentHoleCards;
    std::string m_statusText;
    std::string m_playerBestHandText;
    std::string m_winningSummaryText;
};
