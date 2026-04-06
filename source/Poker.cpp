// Description: Implementation file for the Poker game class.
// Related Files:
// Date Created: 3/29/2026
// Last Edited: 3/29/2026

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>
#include <map>
#include <random>
#include <string>
#include <vector>
#include "Poker.h"


// -----------------------------------------
// Hand
// -----------------------------------------

void Poker::Hand::addCard(const Card& card) {
    m_cards.push_back(card);
}

int Poker::Hand::getValue() const {
    int total = 0;
    for (const auto& c : m_cards) total += c.value;
    return total;
}

void Poker::Hand::clear() {
    m_cards.clear();
}

void Poker::Hand::show(bool hideFirstCard) const {
    for (size_t i = 0; i < m_cards.size(); ++i) {
        if (i == 0 && hideFirstCard)
            std::cout << "[??] ";
        else
            std::cout << "[" << m_cards[i].rank << m_cards[i].suit << "] ";
    }
    std::cout << "\n";
}

const std::vector<Poker::Card>& Poker::Hand::getCards() const {
    return m_cards;
}

int Poker::Hand::size() const {
    return static_cast<int>(m_cards.size());
}

// -----------------------------------------
// Deck
// -----------------------------------------

Poker::Deck::Deck() {
    std::random_device rd;
    m_rng.seed(rd());
    reset();
    shuffle();
}

void Poker::Deck::reset() {
    m_cards.clear();
    const std::vector<std::string> ranks = {
        "2","3","4","5","6","7","8","9","10","J","Q","K","A"
    };
    const std::vector<std::string> suits = { "♥","♦","♣","♠" };
    const std::vector<int> values = {
        2,3,4,5,6,7,8,9,10,10,10,10,11
    };

    for (size_t r = 0; r < ranks.size(); ++r)
        for (const auto& s : suits)
            m_cards.push_back({ ranks[r], s, values[r] });
}

void Poker::Deck::shuffle() {
    std::shuffle(m_cards.begin(), m_cards.end(), m_rng);
}

Poker::Card Poker::Deck::draw() {
    if (m_cards.empty()) { reset(); shuffle(); }
    Card c = m_cards.back();
    m_cards.pop_back();
    return c;
}

// -----------------------------------------
// Participant
// -----------------------------------------

void Poker::Participant::hit(Deck& deck) {
    m_hand.addCard(deck.draw());
}

int Poker::Participant::getHandValue() const {
    return m_hand.getValue();
}

void Poker::Participant::showHand(bool hideFirstCard) const {
    m_hand.show(hideFirstCard);
}

const Poker::Hand& Poker::Participant::getHand() const {
    return m_hand;
}

void Poker::Participant::clearHand() {
    m_hand.clear();
}

// -----------------------------------------
// PokerPlayer
// -----------------------------------------

void Poker::PokerPlayer::takeTurn(Deck& deck) {
    m_hand.addCard(deck.draw());
}

void Poker::PokerPlayer::discardAndDraw(Deck& deck) {
    // Not used in Hold'em but kept for future modes
}

// -----------------------------------------
// AIOpponent
// -----------------------------------------

Poker::AIOpponent::AIOpponent(std::string name)
    : m_name(std::move(name)) {}

const std::string& Poker::AIOpponent::getName() const {
    return m_name;
}

void Poker::AIOpponent::takeTurn(Deck& deck) {
    m_hand.addCard(deck.draw());
}

// -----------------------------------------
// Hand Evaluation
// -----------------------------------------

Poker::HandRank Poker::evaluateHand(const Hand& hand) const {
    const auto& cards = hand.getCards();
    if (cards.size() < 5) return HandRank::HighCard;

    // Count ranks and suits
    std::map<int, int> rankCount;
    std::map<std::string, int> suitCount;
    for (const auto& c : cards) {
        rankCount[c.value]++;
        suitCount[c.suit]++;
    }

    bool isFlush = false;
    for (const auto& [suit, count] : suitCount)
        if (count >= 5) { isFlush = true; break; }

    // Build sorted unique values for straight detection
    std::vector<int> vals;
    for (const auto& [v, _] : rankCount) vals.push_back(v);
    std::sort(vals.begin(), vals.end());

    // Check straight (handle A-low: A=11 treated as 1)
    auto checkStraight = [&](const std::vector<int>& v) -> bool {
        if (v.size() < 5) return false;
        for (size_t i = v.size() - 1; i >= 4; --i)
            if (v[i] - v[i-4] == 4 &&
                v[i-1]-v[i-4]==3 &&
                v[i-2]-v[i-4]==2 &&
                v[i-3]-v[i-4]==1)
                return true;
        return false;
    };

    bool isStraight = checkStraight(vals);
    // A-low straight (A,2,3,4,5)
    if (!isStraight) {
        bool hasAce = rankCount.count(11);
        if (hasAce) {
            std::vector<int> lowVals;
            for (int v : vals) lowVals.push_back(v == 11 ? 1 : v);
            std::sort(lowVals.begin(), lowVals.end());
            isStraight = checkStraight(lowVals);
        }
    }

    // Count pairs/trips/quads
    int pairs = 0, threes = 0, fours = 0;
    for (const auto& [v, c] : rankCount) {
        if (c == 2) pairs++;
        else if (c == 3) threes++;
        else if (c == 4) fours++;
    }

    if (isStraight && isFlush) {
        // Check royal (10,J,Q,K,A)
        if (rankCount.count(10) && rankCount.count(11))
            return HandRank::RoyalFlush;
        return HandRank::StraightFlush;
    }
    if (fours)                      return HandRank::FourOfAKind;
    if (threes && pairs)            return HandRank::FullHouse;
    if (isFlush)                    return HandRank::Flush;
    if (isStraight)                 return HandRank::Straight;
    if (threes)                     return HandRank::ThreeOfAKind;
    if (pairs >= 2)                 return HandRank::TwoPair;
    if (pairs == 1)                 return HandRank::OnePair;
    return HandRank::HighCard;
}

std::string Poker::handRankToString(HandRank rank) const {
    switch (rank) {
        case HandRank::RoyalFlush:    return "Royal Flush";
        case HandRank::StraightFlush: return "Straight Flush";
        case HandRank::FourOfAKind:   return "Four of a Kind";
        case HandRank::FullHouse:     return "Full House";
        case HandRank::Flush:         return "Flush";
        case HandRank::Straight:      return "Straight";
        case HandRank::ThreeOfAKind:  return "Three of a Kind";
        case HandRank::TwoPair:       return "Two Pair";
        case HandRank::OnePair:       return "One Pair";
        default:                      return "High Card";
    }
}

bool Poker::winsAgainst(const Hand& a, const Hand& b) const {
    HandRank ra = evaluateHand(a);
    HandRank rb = evaluateHand(b);
    if (ra != rb) return ra > rb;

    // Tiebreak: highest card value
    auto highCard = [](const Hand& h) {
        int best = 0;
        for (const auto& c : h.getCards())
            best = std::max(best, c.value);
        return best;
    };
    return highCard(a) >= highCard(b);
}

// -----------------------------------------
// Display Helpers
// -----------------------------------------

void Poker::showBanner() const {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════╗\n";
    std::cout << "|     TEXAS HOLD'EM POKER          |\n";
    std::cout << "╚══════════════════════════════════╝\n\n";
}

void Poker::showAllHands(bool revealOpponents) const {
    std::cout << "\n-- Player Hand --\n";
    m_player.showHand();

    for (const auto& opp : m_opponents) {
        std::cout << "\n-- " << opp.getName() << "'s Hand --\n";
        opp.showHand(!revealOpponents);
    }
    std::cout << "\n";
}

double Poker::getBet(const Player& player) const {
    double amount = 0.0;
    while (true) {
        std::cout << "Your balance: $" << std::fixed << std::setprecision(2)
                  << player.getBalance() << "\n";
        std::cout << "Enter bet amount: $";
        std::cin >> amount;

        if (std::cin.fail() || amount <= 0) {
            std::cout << "Invalid amount.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else if (amount > player.getBalance()) {
            std::cout << "Insufficient funds.\n";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return amount;
        }
    }
}

template <typename T>
T Poker::getValidatedInput(T min, T max) const {
    T input;
    while (true) {
        std::cin >> input;
        if (!std::cin.fail() && input >= min && input <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return input;
        }
        std::cout << "Invalid input. Enter a value between "
                  << min << " and " << max << ": ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// -----------------------------------------
// Betting Round
// -----------------------------------------

// Returns the total amount added to the pot by the player this round.
// foldedOpponents tracks which opponents have folded.
double Poker::bettingRound(Player& player, double& pot,
                            std::vector<bool>& foldedOpponents,
                            double currentBet, bool isPreFlop,
                            const std::vector<Card>& community) {
    // AI actions first
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> bluffDist(1, 10);

    for (size_t i = 0; i < m_opponents.size(); ++i) {
        if (foldedOpponents[i]) continue;

        Hand full = m_opponents[i].getHand();
        for (const auto& c : community) full.addCard(c);
        HandRank aiRank = evaluateHand(full);

        int bluff = bluffDist(rng);
        bool isBluffing = (bluff == 1);

        // Pre-flop: never fold, always at least call
        bool isPreFlop = (m_opponents[i].getHand().size() == 2);

        if (isBluffing) {
            double raise = std::max(currentBet + 10.0, 10.0);;
            std::cout << m_opponents[i].getName() << " raises to $"
                    << std::fixed << std::setprecision(2) << raise << ".\n";
            pot += raise;
            currentBet = raise;
        } else if (isPreFlop || aiRank >= HandRank::OnePair) {
            if (aiRank >= HandRank::ThreeOfAKind) {
                double raise = std::max(currentBet + 20.0, 20.0);;
                std::cout << m_opponents[i].getName() << " raises to $"
                        << raise << ".\n";
                pot += raise;
                currentBet = raise;
            } else {
                if (currentBet == 0.0)
                    std::cout << m_opponents[i].getName() << " checks.\n";
                else {
                    std::cout << m_opponents[i].getName() << " calls.\n";
                    pot += currentBet;
                }
            }
        } else {
            std::cout << m_opponents[i].getName() << " folds.\n";
            foldedOpponents[i] = true;
        }
    }

    // Player action
    std::cout << "\nPot: $" << std::fixed << std::setprecision(2) << pot << "\n";
    std::cout << "Current bet to call: $" << currentBet << "\n";
    std::cout << "Your balance: $" << player.getBalance() << "\n\n";
    if (currentBet == 0.0) {
        std::cout << "1. Fold\n2. Check\n3. Raise\n4. All-in\n";
    } else {
        std::cout << "1. Fold\n2. Call ($" << currentBet << ")\n3. Raise\n4. All-in\n";
    }
    std::cout << "Choice: ";

    int choice = getValidatedInput<int>(1, 4);
    double playerContribution = 0.0;

    switch (choice) {
        case 1: // Fold
            std::cout << "You fold.\n";
            return -1.0; // sentinel for fold

        case 2: // Call
            if (currentBet == 0.0) {
                std::cout << "You check.\n";
                // nothing added to pot
            } else {
                if (player.placeBet(currentBet)) {
                    pot += currentBet;
                    playerContribution = currentBet;
                    std::cout << "You call $" << currentBet << ".\n";
                } else {
                    std::cout << "Not enough funds. Going all-in.\n";
                    pot += player.getBalance();
                    playerContribution = player.getBalance();
                    player.placeBet(player.getBalance());
                }
            }
            break;

        case 3: { // Raise
            std::cout << "Enter raise amount (added on top of current bet $" << currentBet << "): $";
            double raiseExtra = getValidatedInput<double>(1.0, player.getBalance() - currentBet);
            double totalRaise = currentBet + raiseExtra;
            if (player.placeBet(totalRaise)) {
                pot += totalRaise;
                playerContribution = totalRaise;
                std::cout << "You raise to $" << totalRaise << ".\n";
            }
            break;
        }

        case 4: // All-in
            pot += player.getBalance();
            playerContribution = player.getBalance();
            player.placeBet(player.getBalance());
            std::cout << "You go all-in!\n";
            break;
    }

    return playerContribution;
}

// -----------------------------------------
// Deal Initial Hands
// -----------------------------------------

void Poker::dealInitialHands() {
    m_player.clearHand();
    for (auto& opp : m_opponents) opp.clearHand();

    // Deal 2 hole cards each
    for (int i = 0; i < 2; ++i) {
        m_player.hit(m_deck);
        for (auto& opp : m_opponents) opp.hit(m_deck);
    }
}

// -----------------------------------------
// Constructor
// -----------------------------------------

Poker::Poker()
    : m_opponents({ AIOpponent("Dealer Dan"),
                    AIOpponent("Reckless Roy"),
                    AIOpponent("Careful Carl") }) {}

// -----------------------------------------
// play()
// -----------------------------------------

void Poker::play(Player& player) {
    showBanner();

    while (true) {
        std::cout << "Balance: $" << std::fixed << std::setprecision(2)
                  << player.getBalance() << "\n";
        std::cout << "1. Play Hand\n2. Quit\nChoice: ";
        int menuChoice = getValidatedInput<int>(1, 2);
        if (menuChoice == 2) break;
        if (player.getBalance() <= 0.0) {
            std::cout << "You're out of funds!\n";
            break;
        }

        // -- Ante --
        std::cout << "\nAnte up! Enter your initial bet:\n";
        double ante = getBet(player);
        player.placeBet(ante);
        double pot = ante;

        // -- Deal hole cards --
        m_deck.reset();
        m_deck.shuffle();
        dealInitialHands();

        std::cout << "\n-- Your Hole Cards --\n";
        m_player.showHand();

        // -- Community cards --
        std::vector<Card> community;

        // Pre-flop betting
        std::cout << "\n-- Pre-Flop Betting --\n";
        std::vector<bool> folded(m_opponents.size(), false);
        double result = bettingRound(player, pot, folded, ante, true, community);
        if (result < 0.0) {
            std::cout << "You folded. Pot goes to opponents.\n\n";
            continue;
        }

        // Flop (3 cards)
        for (int i = 0; i < 3; ++i) community.push_back(m_deck.draw());
        std::cout << "\n-- The Flop --\n";
        for (const auto& c : community)
            std::cout << "[" << c.rank << c.suit << "] ";
        std::cout << "\n";

        // Give community cards to all participants for evaluation
        auto addCommunity = [&](Participant& p) {
            for (const auto& c : community) p.getHand(); // view only
        };
        // Build combined hands for evaluation (hole + community)
        auto buildFull = [&](const Hand& hole) -> Hand {
            Hand full = hole;
            for (const auto& c : community) full.addCard(c);
            return full;
        };

        // Post-flop betting
        std::cout << "\n-- Post-Flop Betting --\n";
        result = bettingRound(player, pot, folded, 0.0, false, community);
        if (result < 0.0) {
            std::cout << "You folded. Pot goes to opponents.\n\n";
            continue;
        }

        // Turn (1 card)
        community.push_back(m_deck.draw());
        std::cout << "\n-- The Turn --\n";
        for (const auto& c : community)
            std::cout << "[" << c.rank << c.suit << "] ";
        std::cout << "\n";

        // Turn betting
        std::cout << "\n-- Turn Betting --\n";
        result = bettingRound(player, pot, folded, 0.0, false, community);
        if (result < 0.0) {
            std::cout << "You folded. Pot goes to opponents.\n\n";
            continue;
        }

        // River (1 card)
        community.push_back(m_deck.draw());
        std::cout << "\n-- The River --\n";
        for (const auto& c : community)
            std::cout << "[" << c.rank << c.suit << "] ";
        std::cout << "\n";

        // River betting
        std::cout << "\n-- River Betting --\n";
        result = bettingRound(player, pot, folded, 0.0, false, community);
        if (result < 0.0) {
            std::cout << "You folded. Pot goes to opponents.\n\n";
            continue;
        }

        // -- Showdown --
        std::cout << "\n== SHOWDOWN ==\n";
        showAllHands(true);

        Hand playerFull = buildFull(m_player.getHand());
        HandRank playerRank = evaluateHand(playerFull);
        std::cout << "Your hand: " << handRankToString(playerRank) << "\n";

        bool playerWins = true;
        for (size_t i = 0; i < m_opponents.size(); ++i) {
            if (folded[i]) continue;
            Hand oppFull = buildFull(m_opponents[i].getHand());
            HandRank oppRank = evaluateHand(oppFull);
            std::cout << m_opponents[i].getName() << ": "
                      << handRankToString(oppRank) << "\n";

            if (!winsAgainst(playerFull, oppFull)) {
                playerWins = false;
            }
        }

        if (playerWins) {
            std::cout << "\nYou win the pot of $" << std::fixed
                      << std::setprecision(2) << pot << "!\n";
            player.addWinnings(pot);
        } else {
            std::cout << "\nYou lose this hand.\n";
        }

        std::cout << "\nBalance: $" << player.getBalance() << "\n\n";
    }
}