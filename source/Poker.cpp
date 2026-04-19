// Description: Implementation file for the Poker game class.
// Related Files:
// Date Created: 3/29/2026
// Last Edited: 4/19/2026

#include "Poker.h"

#include <algorithm>
#include <array>
#include <iomanip>
#include <map>
#include <sstream>
#include <unordered_map>

namespace
{
    std::string formatMoney(double amount)
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << amount;
        return stream.str();
    }
}

void Poker::Hand::addCard(const Card& card)
{
    m_cards.push_back(card);
}

void Poker::Hand::clear()
{
    m_cards.clear();
}

const std::vector<Poker::Card>& Poker::Hand::getCards() const
{
    return m_cards;
}

Poker::Deck::Deck()
    : m_rng(std::random_device{}())
{
    reset();
}

void Poker::Deck::reset()
{
    m_cards.clear();

    const std::vector<std::string> suits = {
        "Hearts", "Diamonds", "Clubs", "Spades"
    };

    const std::vector<std::pair<std::string, int>> ranks = {
        {"2", 2}, {"3", 3}, {"4", 4}, {"5", 5}, {"6", 6},
        {"7", 7}, {"8", 8}, {"9", 9}, {"10", 10},
        {"Jack", 11}, {"Queen", 12}, {"King", 13}, {"Ace", 14}
    };

    for (const std::string& suit : suits)
    {
        for (const auto& [rank, value] : ranks)
        {
            m_cards.push_back({ rank, suit, value });
        }
    }

    shuffle();
}

void Poker::Deck::shuffle()
{
    std::shuffle(m_cards.begin(), m_cards.end(), m_rng);
}

Poker::Card Poker::Deck::draw()
{
    if (m_cards.empty())
        reset();

    Card card = m_cards.back();
    m_cards.pop_back();
    return card;
}

Poker::Poker()
    : m_rng(std::random_device{}()),
    m_opponents({
        Opponent{ "Dealer Dan", {}, false, 0.0, 0.0, "" },
        Opponent{ "Reckless Roy", {}, false, 0.0, 0.0, "" },
        Opponent{ "Careful Carl", {}, false, 0.0, 0.0, "" }
        }),
    m_ante(0.0),
    m_pot(0.0),
    m_currentBet(0.0),
    m_playerContribution(0.0),
    m_playerTotalCommitted(0.0),
    m_roundState(RoundState::WaitingForBet),
    m_phase(Phase::None),
    m_roundResult(RoundResult::None),
... (887 lines left)

message.txt
26 KB
lmk when u commit em
so i can commit the rest
﻿
icarus
7icaarus
 
 
icarus.exe —⠀❐⠀⤬
// Description: Implementation file for the Poker game class.
// Related Files:
// Date Created: 3/29/2026
// Last Edited: 4/19/2026

#include "Poker.h"

#include <algorithm>
#include <array>
#include <iomanip>
#include <map>
#include <sstream>
#include <unordered_map>

namespace
{
    std::string formatMoney(double amount)
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << amount;
        return stream.str();
    }
}

void Poker::Hand::addCard(const Card& card)
{
    m_cards.push_back(card);
}

void Poker::Hand::clear()
{
    m_cards.clear();
}

const std::vector<Poker::Card>& Poker::Hand::getCards() const
{
    return m_cards;
}

Poker::Deck::Deck()
    : m_rng(std::random_device{}())
{
    reset();
}

void Poker::Deck::reset()
{
    m_cards.clear();

    const std::vector<std::string> suits = {
        "Hearts", "Diamonds", "Clubs", "Spades"
    };

    const std::vector<std::pair<std::string, int>> ranks = {
        {"2", 2}, {"3", 3}, {"4", 4}, {"5", 5}, {"6", 6},
        {"7", 7}, {"8", 8}, {"9", 9}, {"10", 10},
        {"Jack", 11}, {"Queen", 12}, {"King", 13}, {"Ace", 14}
    };

    for (const std::string& suit : suits)
    {
        for (const auto& [rank, value] : ranks)
        {
            m_cards.push_back({ rank, suit, value });
        }
    }

    shuffle();
}

void Poker::Deck::shuffle()
{
    std::shuffle(m_cards.begin(), m_cards.end(), m_rng);
}

Poker::Card Poker::Deck::draw()
{
    if (m_cards.empty())
        reset();

    Card card = m_cards.back();
    m_cards.pop_back();
    return card;
}

Poker::Poker()
    : m_rng(std::random_device{}()),
    m_opponents({
        Opponent{ "Dealer Dan", {}, false, 0.0, 0.0, "" },
        Opponent{ "Reckless Roy", {}, false, 0.0, 0.0, "" },
        Opponent{ "Careful Carl", {}, false, 0.0, 0.0, "" }
        }),
    m_ante(0.0),
    m_pot(0.0),
    m_currentBet(0.0),
    m_playerContribution(0.0),
    m_playerTotalCommitted(0.0),
    m_roundState(RoundState::WaitingForBet),
    m_phase(Phase::None),
    m_roundResult(RoundResult::None),
    m_hideOpponentHoleCards(true),
    m_statusText("Enter your ante and press Deal."),
    m_playerBestHandText(""),
    m_winningSummaryText("")
{
}

void Poker::resetRound()
{
    m_deck.reset();
    m_playerHand.clear();
    m_communityCards.clear();

    for (Opponent& opponent : m_opponents)
    {
        opponent.hand.clear();
        opponent.folded = false;
        opponent.contribution = 0.0;
        opponent.totalCommitted = 0.0;
        opponent.bestHandText.clear();
    }

    m_ante = 0.0;
    m_pot = 0.0;
    m_currentBet = 0.0;
    m_playerContribution = 0.0;
    m_playerTotalCommitted = 0.0;

    m_roundState = RoundState::WaitingForBet;
    m_phase = Phase::None;
    m_roundResult = RoundResult::None;
    m_hideOpponentHoleCards = true;
    m_statusText = "Enter your ante and press Deal.";
    m_playerBestHandText.clear();
    m_winningSummaryText.clear();
}

bool Poker::startRound(Player& player, double ante)
{
    if (m_roundState == RoundState::PlayerTurn || m_roundState == RoundState::Showdown)
    {
        m_statusText = "Finish the current hand first.";
        return false;
    }

    if (ante <= 0.0)
    {
        m_statusText = "Ante must be greater than $0.00.";
        return false;
    }

    if (!player.placeBet(ante))
    {
        m_statusText = "Invalid ante or not enough balance.";
        return false;
    }

    m_deck.reset();
    m_playerHand.clear();
    m_communityCards.clear();

    for (Opponent& opponent : m_opponents)
    {
        opponent.hand.clear();
        opponent.folded = false;
        opponent.contribution = 0.0;
        opponent.totalCommitted = ante;
        opponent.bestHandText.clear();
    }

    m_ante = ante;
    m_pot = ante * (1.0 + static_cast<double>(m_opponents.size()));
    m_currentBet = 0.0;
    m_playerContribution = 0.0;
    m_playerTotalCommitted = ante;

    m_roundState = RoundState::PlayerTurn;
    m_phase = Phase::PreFlop;
    m_roundResult = RoundResult::None;
    m_hideOpponentHoleCards = true;
    m_playerBestHandText.clear();
    m_winningSummaryText.clear();

    dealHoleCards();
    m_statusText = "Pre-Flop. Check, raise, or fold.";
    return true;
}

void Poker::dealHoleCards()
{
    for (int cardIndex = 0; cardIndex < 2; ++cardIndex)
    {
        m_playerHand.addCard(m_deck.draw());

        for (Opponent& opponent : m_opponents)
            opponent.hand.addCard(m_deck.draw());
    }
}

void Poker::beginPhase(Phase phase)
{
    m_phase = phase;
    m_currentBet = 0.0;
    m_playerContribution = 0.0;

    for (Opponent& opponent : m_opponents)
        opponent.contribution = 0.0;

    m_roundState = RoundState::PlayerTurn;
}

void Poker::dealFlop()
{
    for (int i = 0; i < 3; ++i)
        m_communityCards.push_back(m_deck.draw());
}

void Poker::dealTurn()
{
    m_communityCards.push_back(m_deck.draw());
}

void Poker::dealRiver()
{
    m_communityCards.push_back(m_deck.draw());
}

void Poker::advanceToNextPhase(Player& player, const std::string& prefixStatus)
{
    int activeOpponents = 0;
    for (const Opponent& opponent : m_opponents)
    {
        if (!opponent.folded)
            ++activeOpponents;
    }

    if (activeOpponents <= 0)
    {
        settlePlayerWin(player, prefixStatus + " Everyone else folded.");
        return;
    }

    switch (m_phase)
    {
    case Phase::PreFlop:
        dealFlop();
        beginPhase(Phase::Flop);
        m_statusText = prefixStatus + " Flop dealt. Check, raise, or fold.";
        return;

    case Phase::Flop:
        dealTurn();
        beginPhase(Phase::Turn);
        m_statusText = prefixStatus + " Turn dealt. Check, raise, or fold.";
        return;

    case Phase::Turn:
        dealRiver();
        beginPhase(Phase::River);
        m_statusText = prefixStatus + " River dealt. Check, raise, or fold.";
        return;

    case Phase::River:
        settleShowdown(player);
        return;

    case Phase::Showdown:
    case Phase::None:
    default:
        return;
    }
}

void Poker::playerFold(Player& player)
{
    (void)player;

    if (m_roundState != RoundState::PlayerTurn)
        return;

    settlePlayerFold();
}

void Poker::playerCheckCall(Player& player)
{
    if (m_roundState != RoundState::PlayerTurn)
        return;

    const double amountToCall = std::max(0.0, m_currentBet - m_playerContribution);

    if (amountToCall > 0.0)
    {
        if (!player.placeBet(amountToCall))
        {
            m_statusText = "Not enough balance to call.";
            return;
        }

        m_playerContribution += amountToCall;
        m_playerTotalCommitted += amountToCall;
        m_pot += amountToCall;

        advanceToNextPhase(player, "You called $" + formatMoney(amountToCall) + ".");
        return;
    }

    resolveOpponentsAfterPlayerCheck(player);
}

void Poker::playerRaise(Player& player, double raiseAmount)
{
    if (m_roundState != RoundState::PlayerTurn)
        return;

    if (raiseAmount <= 0.0)
    {
        m_statusText = "Raise must be greater than $0.00.";
        return;
    }

    const double amountToCall = std::max(0.0, m_currentBet - m_playerContribution);
    const double totalAmount = amountToCall + raiseAmount;

    if (!player.placeBet(totalAmount))
    {
        m_statusText = "Not enough balance for that raise.";
        return;
    }

    m_playerContribution += totalAmount;
    m_playerTotalCommitted += totalAmount;
    m_pot += totalAmount;
    m_currentBet = std::max(m_currentBet, m_playerContribution);

    resolveOpponentsFacingPlayerBet(player);
}

void Poker::resolveOpponentsAfterPlayerCheck(Player& player)
{
    int bettorIndex = -1;

    for (std::size_t i = 0; i < m_opponents.size(); ++i)
    {
        if (m_opponents[i].folded)
            continue;

        if (shouldOpponentLeadBet(m_opponents[i]))
        {
            bettorIndex = static_cast<int>(i);
            break;
        }
    }

    if (bettorIndex < 0)
    {
        advanceToNextPhase(player, "Everyone checks.");
        return;
    }

    Opponent& bettor = m_opponents[static_cast<std::size_t>(bettorIndex)];
    const double betSize = chooseOpponentBetSize();

    bettor.contribution = betSize;
    bettor.totalCommitted += betSize;

    m_currentBet = betSize;
    m_pot += betSize;

    for (std::size_t i = 0; i < m_opponents.size(); ++i)
    {
        if (static_cast<int>(i) == bettorIndex)
            continue;

        Opponent& opponent = m_opponents[i];
        if (opponent.folded)
            continue;

        if (shouldOpponentFoldToBet(opponent, betSize))
        {
            opponent.folded = true;
            continue;
        }

        opponent.contribution = betSize;
        opponent.totalCommitted += betSize;
        m_pot += betSize;
    }

    maybeAwardPotIfEveryoneFolded(player);
    if (m_roundState == RoundState::RoundOver)
        return;

    m_statusText = bettor.name + " bets $" + formatMoney(betSize) + ". Call, raise, or fold.";
}

void Poker::resolveOpponentsFacingPlayerBet(Player& player)
{
    int callers = 0;
    int folders = 0;

    for (Opponent& opponent : m_opponents)
    {
        if (opponent.folded)
            continue;

        const double amountToCall = std::max(0.0, m_currentBet - opponent.contribution);

        if (amountToCall <= 0.0)
        {
            ++callers;
            continue;
        }

        if (shouldOpponentFoldToBet(opponent, amountToCall))
        {
            opponent.folded = true;
            ++folders;
            continue;
        }

        opponent.contribution += amountToCall;
        opponent.totalCommitted += amountToCall;
        m_pot += amountToCall;
        ++callers;
    }

    maybeAwardPotIfEveryoneFolded(player);
    if (m_roundState == RoundState::RoundOver)
        return;

    std::ostringstream stream;
    stream << "You raised to $" << std::fixed << std::setprecision(2) << m_currentBet << ". ";

    if (callers > 0)
        stream << callers << " opponent" << (callers == 1 ? "" : "s") << " called. ";

    if (folders > 0)
        stream << folders << " folded. ";

    advanceToNextPhase(player, stream.str());
}

void Poker::maybeAwardPotIfEveryoneFolded(Player& player)
{
    for (const Opponent& opponent : m_opponents)
    {
        if (!opponent.folded)
            return;
    }

    settlePlayerWin(player, "Everyone folded.");
}

int Poker::estimateOpponentStrength(const Opponent& opponent) const
{
    std::vector<Card> combined = buildCombinedCards(opponent.hand);
    const EvaluatedHand evaluated = evaluateBestHand(combined);

    int score = static_cast<int>(evaluated.rank) * 25;
    if (!evaluated.tieBreakers.empty())
        score += evaluated.tieBreakers.front();

    const auto& holeCards = opponent.hand.getCards();
    if (holeCards.size() == 2)
    {
        if (holeCards[0].value == holeCards[1].value)
            score += 12;

        if (holeCards[0].value >= 11)
            score += 4;
        if (holeCards[1].value >= 11)
            score += 4;

        if (holeCards[0].suit == holeCards[1].suit)
            score += 3;
    }

    return score;
}

bool Poker::shouldOpponentLeadBet(const Opponent& opponent) const
{
    const int strength = estimateOpponentStrength(opponent);

    if (strength >= 90)
        return true;

    if (strength >= 70)
        return (const_cast<Poker*>(this)->randomPercent() < 85);

    if (strength >= 50)
        return (const_cast<Poker*>(this)->randomPercent() < 45);

    return (const_cast<Poker*>(this)->randomPercent() < 15);
}

bool Poker::shouldOpponentFoldToBet(const Opponent& opponent, double amountToCall) const
{
    const int strength = estimateOpponentStrength(opponent);
    const double aggressionUnit = std::max(m_ante, 5.0);
    const double pressure = amountToCall / aggressionUnit;

    if (strength >= 105)
        return false;

    if (strength >= 80)
        return pressure > 5.0 && (const_cast<Poker*>(this)->randomPercent() < 25);

    if (strength >= 60)
        return pressure > 3.0 && (const_cast<Poker*>(this)->randomPercent() < 50);

    if (strength >= 40)
        return pressure > 2.0 || (const_cast<Poker*>(this)->randomPercent() < 45);

    return pressure > 1.0 || (const_cast<Poker*>(this)->randomPercent() < 70);
}

double Poker::chooseOpponentBetSize() const
{
    const double base = std::max(m_ante, 5.0);

    switch (m_phase)
    {
    case Phase::PreFlop:
        return base;

    case Phase::Flop:
        return base * 1.25;

    case Phase::Turn:
        return base * 1.5;

    case Phase::River:
        return base * 2.0;

    case Phase::Showdown:
    case Phase::None:
    default:
        return base;
    }
}

int Poker::randomPercent()
{
    std::uniform_int_distribution<int> distribution(0, 99);
    return distribution(m_rng);
}

void Poker::settlePlayerFold()
{
    m_roundResult = RoundResult::PlayerFolded;
    m_roundState = RoundState::RoundOver;
    m_phase = Phase::Showdown;
    m_hideOpponentHoleCards = false;
    m_playerBestHandText.clear();
    m_winningSummaryText = "You folded. Opponents take the pot.";
    m_statusText = m_winningSummaryText;
}

void Poker::settlePlayerWin(Player& player, const std::string& statusText)
{
    m_roundResult = RoundResult::PlayerWin;
    m_roundState = RoundState::RoundOver;
    m_phase = Phase::Showdown;
    m_hideOpponentHoleCards = false;
    player.addWinnings(m_pot);
    m_winningSummaryText = "You win $" + formatMoney(m_pot) + ".";
    m_statusText = statusText + " " + m_winningSummaryText;
}

void Poker::settleShowdown(Player& player)
{
    m_phase = Phase::Showdown;
    m_roundState = RoundState::Showdown;
    m_hideOpponentHoleCards = false;

    const EvaluatedHand playerHand = evaluateBestHand(buildCombinedCards(m_playerHand));
    m_playerBestHandText = handRankToString(playerHand.rank);

    for (Opponent& opponent : m_opponents)
    {
        if (opponent.folded)
        {
            opponent.bestHandText = "Folded";
            continue;
        }

        const EvaluatedHand evaluated = evaluateBestHand(buildCombinedCards(opponent.hand));
        opponent.bestHandText = handRankToString(evaluated.rank);
    }

    std::vector<std::string> winners;
    EvaluatedHand bestHand = playerHand;
    winners.push_back("You");

    for (const Opponent& opponent : m_opponents)
    {
        if (opponent.folded)
            continue;

        const EvaluatedHand evaluated = evaluateBestHand(buildCombinedCards(opponent.hand));

        if (isBetterHand(evaluated, bestHand))
        {
            bestHand = evaluated;
            winners.clear();
            winners.push_back(opponent.name);
        }
        else if (isEqualHand(evaluated, bestHand))
        {
            winners.push_back(opponent.name);
        }
    }

    const bool playerIncluded =
        std::find(winners.begin(), winners.end(), "You") != winners.end();

    if (playerIncluded)
    {
        const double playerShare = m_pot / static_cast<double>(winners.size());
        player.addWinnings(playerShare);

        if (winners.size() == 1)
        {
            m_roundResult = RoundResult::PlayerWin;
            m_winningSummaryText = "You win $" + formatMoney(m_pot) + " with " + m_playerBestHandText + ".";
        }
        else
        {
            m_roundResult = RoundResult::SplitPot;
            m_winningSummaryText = "Split pot between ";
            for (std::size_t i = 0; i < winners.size(); ++i)
            {
                if (i > 0)
                    m_winningSummaryText += (i + 1 == winners.size()) ? " and " : ", ";

                m_winningSummaryText += winners[i];
            }

            m_winningSummaryText += ". Your share: $" + formatMoney(playerShare) + ".";
        }
    }
    else
    {
        m_roundResult = RoundResult::OpponentWin;
        m_winningSummaryText = "Winner";
        if (winners.size() > 1)
            m_winningSummaryText += "s";

        m_winningSummaryText += ": ";

        for (std::size_t i = 0; i < winners.size(); ++i)
        {
            if (i > 0)
                m_winningSummaryText += (i + 1 == winners.size()) ? " and " : ", ";

            m_winningSummaryText += winners[i];
        }

        m_winningSummaryText += " with " + handRankToString(bestHand.rank) + ".";
    }

    m_statusText = m_winningSummaryText;
    m_roundState = RoundState::RoundOver;
}

std::vector<Poker::Card> Poker::buildCombinedCards(const Hand& holeCards) const
{
    std::vector<Card> cards = holeCards.getCards();
    cards.insert(cards.end(), m_communityCards.begin(), m_communityCards.end());
    return cards;
}

Poker::EvaluatedHand Poker::evaluateBestHand(const std::vector<Card>& cards) const
{
    if (cards.size() < 5)
        return { HandRank::HighCard, {} };

    EvaluatedHand best = { HandRank::HighCard, {0, 0, 0, 0, 0} };

    for (std::size_t a = 0; a < cards.size() - 4; ++a)
    {
        for (std::size_t b = a + 1; b < cards.size() - 3; ++b)
        {
            for (std::size_t c = b + 1; c < cards.size() - 2; ++c)
            {
                for (std::size_t d = c + 1; d < cards.size() - 1; ++d)
                {
                    for (std::size_t e = d + 1; e < cards.size(); ++e)
                    {
                        std::vector<Card> hand = {
                            cards[a], cards[b], cards[c], cards[d], cards[e]
                        };

                        const EvaluatedHand candidate = evaluateFiveCardHand(hand);
                        if (isBetterHand(candidate, best))
                            best = candidate;
                    }
                }
            }
        }
    }

    return best;
}

Poker::EvaluatedHand Poker::evaluateFiveCardHand(const std::vector<Card>& cards) const
{
    std::vector<int> values;
    values.reserve(cards.size());

    std::unordered_map<int, int> rankCounts;
    std::unordered_map<std::string, int> suitCounts;

    for (const Card& card : cards)
    {
        values.push_back(card.value);
        rankCounts[card.value]++;
        suitCounts[card.suit]++;
    }

    std::sort(values.begin(), values.end(), std::greater<int>());

    bool isFlush = false;
    for (const auto& [suit, count] : suitCounts)
    {
        if (count == 5)
        {
            isFlush = true;
            break;
        }
    }

    std::vector<int> straightValues = values;
    std::sort(straightValues.begin(), straightValues.end(), std::greater<int>());
    straightValues.erase(std::unique(straightValues.begin(), straightValues.end()), straightValues.end());

    if (std::find(straightValues.begin(), straightValues.end(), 14) != straightValues.end())
        straightValues.push_back(1);

    bool isStraight = false;
    int straightHigh = 0;

    for (std::size_t i = 0; i + 4 < straightValues.size(); ++i)
    {
        if (straightValues[i] - 1 == straightValues[i + 1] &&
            straightValues[i + 1] - 1 == straightValues[i + 2] &&
            straightValues[i + 2] - 1 == straightValues[i + 3] &&
            straightValues[i + 3] - 1 == straightValues[i + 4])
        {
            isStraight = true;
            straightHigh = (straightValues[i] == 1) ? 5 : straightValues[i];
            break;
        }
    }

    std::vector<std::pair<int, int>> groups;
    groups.reserve(rankCounts.size());

    for (const auto& [value, count] : rankCounts)
        groups.push_back({ count, value });

    std::sort(groups.begin(), groups.end(),
        [](const std::pair<int, int>& left, const std::pair<int, int>& right)
        {
            if (left.first != right.first)
                return left.first > right.first;
            return left.second > right.second;
        });

    if (isStraight && isFlush)
        return { HandRank::StraightFlush, {straightHigh} };

    if (groups[0].first == 4)
    {
        const int quadValue = groups[0].second;
        const int kicker = groups[1].second;
        return { HandRank::FourOfAKind, {quadValue, kicker} };
    }

    if (groups[0].first == 3 && groups[1].first == 2)
        return { HandRank::FullHouse, {groups[0].second, groups[1].second} };

    if (isFlush)
        return { HandRank::Flush, values };

    if (isStraight)
        return { HandRank::Straight, {straightHigh} };

    if (groups[0].first == 3)
    {
        std::vector<int> tieBreakers = { groups[0].second };

        for (const auto& group : groups)
        {
            if (group.first == 1)
                tieBreakers.push_back(group.second);
        }

        return { HandRank::ThreeOfAKind, tieBreakers };
    }

    if (groups[0].first == 2 && groups[1].first == 2)
    {
        const int topPair = std::max(groups[0].second, groups[1].second);
        const int lowerPair = std::min(groups[0].second, groups[1].second);
        const int kicker = groups.size() > 2 ? groups[2].second : 0;

        return { HandRank::TwoPair, {topPair, lowerPair, kicker} };
    }

    if (groups[0].first == 2)
    {
        std::vector<int> tieBreakers = { groups[0].second };

        for (const auto& group : groups)
        {
            if (group.first == 1)
                tieBreakers.push_back(group.second);
        }

        std::sort(tieBreakers.begin() + 1, tieBreakers.end(), std::greater<int>());
        return { HandRank::OnePair, tieBreakers };
    }

    return { HandRank::HighCard, values };
}

bool Poker::isBetterHand(const EvaluatedHand& left, const EvaluatedHand& right)
{
    if (left.rank != right.rank)
        return static_cast<int>(left.rank) > static_cast<int>(right.rank);

    return left.tieBreakers > right.tieBreakers;
}

bool Poker::isEqualHand(const EvaluatedHand& left, const EvaluatedHand& right)
{
    return left.rank == right.rank && left.tieBreakers == right.tieBreakers;
}

std::string Poker::handRankToString(HandRank rank)
{
    switch (rank)
    {
    case HandRank::HighCard:      return "High Card";
    case HandRank::OnePair:       return "One Pair";
    case HandRank::TwoPair:       return "Two Pair";
    case HandRank::ThreeOfAKind:  return "Three of a Kind";
    case HandRank::Straight:      return "Straight";
    case HandRank::Flush:         return "Flush";
    case HandRank::FullHouse:     return "Full House";
    case HandRank::FourOfAKind:   return "Four of a Kind";
    case HandRank::StraightFlush: return "Straight Flush";
    default:                      return "Unknown";
    }
}

std::string Poker::phaseToString(Phase phase)
{
    switch (phase)
    {
    case Phase::PreFlop:  return "Pre-Flop";
    case Phase::Flop:     return "Flop";
    case Phase::Turn:     return "Turn";
    case Phase::River:    return "River";
    case Phase::Showdown: return "Showdown";
    case Phase::None:
    default:              return "Waiting";
    }
}

std::vector<Poker::CardView> Poker::toCardViews(const std::vector<Card>& cards) const
{
    std::vector<CardView> views;
    views.reserve(cards.size());

    for (const Card& card : cards)
        views.push_back({ card.rank, card.suit, card.value });

    return views;
}

Poker::RoundState Poker::getRoundState() const
{
    return m_roundState;
}

Poker::Phase Poker::getPhase() const
{
    return m_phase;
}

Poker::RoundResult Poker::getRoundResult() const
{
    return m_roundResult;
}

std::vector<Poker::CardView> Poker::getPlayerCards() const
{
    return toCardViews(m_playerHand.getCards());
}

std::vector<Poker::CardView> Poker::getCommunityCards() const
{
    return toCardViews(m_communityCards);
}

std::vector<Poker::OpponentView> Poker::getOpponents() const
{
    std::vector<OpponentView> views;
    views.reserve(m_opponents.size());

    for (const Opponent& opponent : m_opponents)
    {
        views.push_back({
            opponent.name,
            toCardViews(opponent.hand.getCards()),
            opponent.folded,
            opponent.contribution,
            opponent.bestHandText
            });
    }

    return views;
}

double Poker::getAnte() const
{
    return m_ante;
}

double Poker::getPot() const
{
    return m_pot;
}

double Poker::getCurrentBet() const
{
    return m_currentBet;
}

double Poker::getAmountToCall() const
{
    return std::max(0.0, m_currentBet - m_playerContribution);
}

double Poker::getPlayerContribution() const
{
    return m_playerContribution;
}

std::string Poker::getStatusText() const
{
    return m_statusText;
}

std::string Poker::getPhaseText() const
{
    return phaseToString(m_phase);
}

std::string Poker::getPlayerBestHandText() const
{
    return m_playerBestHandText;
}

std::string Poker::getWinningSummaryText() const
{
    return m_winningSummaryText;
}

bool Poker::isRoundOver() const
{
    return m_roundState == RoundState::RoundOver;
}

bool Poker::areOpponentHoleCardsHidden() const
{
    return m_hideOpponentHoleCards;
}

bool Poker::isPlayerTurn() const
{
    return m_roundState == RoundState::PlayerTurn;
}