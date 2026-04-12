// Blackjack minigame implementation file.
// Related Files: Blackjack.h, Player.h
// Date Created: 3/29/2026
// Last Edited: 4/04/2026

#include "Blackjack.h"
#include "Player.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>

Blackjack::Blackjack() {}

void Blackjack::play(Player& player) {
    showBanner();

  bool playing = true;
    while (playing) {
        
        double bet = getBet(player);

        if (!player.placeBet(bet)) {
            std::cout << "Bet failed. Not enough funds.\n";
            continue;
        }

        Deck deck;
        BJPlayer bjPlayer;
        Dealer dealer;

        bjPlayer.hit(deck);
        dealer.hit(deck);
        bjPlayer.hit(deck);
        dealer.hit(deck);

        while (true) {
            std::cout << "\nYour hand:\n";
            bjPlayer.showHand();
            std::cout << "Value: " << bjPlayer.getHandValue() << "\n";

            std::cout << "\nDealer shows:\n";
            dealer.showHand(true);

            if (bjPlayer.getHandValue() > 21) {
                std::cout << "\nBust! You lose.\n";
                break;
            }

            std::cout << "\n1. Hit\n2. Stand\nChoice: ";
            int choice = getValidatedInput<int>(1, 2);

            if (choice == 1) {
                bjPlayer.hit(deck);
            } else {
                break;
            }
        }

        if (bjPlayer.getHandValue() <= 21) {
            dealer.takeTurn(deck);

            std::cout << "\nDealer hand:\n";
            dealer.showHand();
            std::cout << "Value: " << dealer.getHandValue() << "\n";
        }

        int playerValue = bjPlayer.getHandValue();
        int dealerValue = dealer.getHandValue();

        if (playerValue > 21) {
            std::cout << "You lost $" << std::fixed << std::setprecision(2) << bet << "\n";
        }
        else if (dealerValue > 21 || playerValue > dealerValue) {
            double winnings = bet * 2.0;
            player.addWinnings(winnings);
            std::cout << "You win $" << winnings << "!\n";
        }
        else if (playerValue == dealerValue) {
            player.addWinnings(bet);
            std::cout << "Push. Bet returned.\n";
        }
        else {
            std::cout << "Dealer wins. You lose $" << bet << "\n";
        }

        std::cout << "Current balance: $" << std::fixed << std::setprecision(2)
                  << player.getBalance() << "\n\n";
        std::cout << "Play again?\n";
std::cout << "1. Yes\n";
std::cout << "2. No (Return to Main Menu)\n";
std::cout << "Choice: ";

int again = getValidatedInput<int>(1, 2);

switch (again) {
case 1:
    break;

case 2:
    std::cout << "Returning to main menu...\n";
    return;
}
    }
}

void Blackjack::Hand::addCard(const Card& card) {
    m_cards.push_back(card);
}

int Blackjack::Hand::getValue() const {
    int total = 0;
    int aces = 0;

    for (const Card& card : m_cards) {
        total += card.value;
        if (card.value == 11) aces++;
    }

    while (total > 21 && aces > 0) {
        total -= 10;
        aces--;
    }

    return total;
}

void Blackjack::Hand::clear() {
    m_cards.clear();
}

void Blackjack::Hand::show(bool hideFirstCard) const {
    for (size_t i = 0; i < m_cards.size(); ++i) {
        if (i == 0 && hideFirstCard) {
            std::cout << "[Hidden Card]\n";
        } else {
            std::cout << m_cards[i].rank << " of " << m_cards[i].suit << "\n";
        }
    }
}

Blackjack::Deck::Deck() : m_rng(std::random_device{}()) {
    reset();
}

void Blackjack::Deck::reset() {
    m_cards.clear();

    std::vector<std::string> suits = {"Hearts", "Diamonds", "Clubs", "Spades"};
    std::vector<std::pair<std::string, int>> ranks = {
        {"2",2},{"3",3},{"4",4},{"5",5},{"6",6},
        {"7",7},{"8",8},{"9",9},{"10",10},
        {"Jack",10},{"Queen",10},{"King",10},{"Ace",11}
    };

    for (const auto& suit : suits) {
        for (const auto& rank : ranks) {
            m_cards.push_back({rank.first, suit, rank.second});
        }
    }

    shuffle();
}

void Blackjack::Deck::shuffle() {
    std::shuffle(m_cards.begin(), m_cards.end(), m_rng);
}

Blackjack::Card Blackjack::Deck::draw() {
    Card card = m_cards.back();
    m_cards.pop_back();
    return card;
}

void Blackjack::Participant::hit(Deck& deck) {
    m_hand.addCard(deck.draw());
}

int Blackjack::Participant::getHandValue() const {
    return m_hand.getValue();
}

void Blackjack::Participant::showHand(bool hideFirstCard) const {
    m_hand.show(hideFirstCard);
}

void Blackjack::BJPlayer::takeTurn(Deck&) {

}

void Blackjack::Dealer::takeTurn(Deck& deck) {
    while (getHandValue() < 17) {
        hit(deck);
    }
}

void Blackjack::showBanner() const {
    std::cout << "============================\n";
    std::cout << "        BLACKJACK           \n";
    std::cout << "============================\n";
}

double Blackjack::getBet(const Player& player) const {
    double bet = 0.0;

    while (true) {
        std::cout << "\nBalance: $" << player.getBalance() << "\n";
        std::cout << "Enter bet: $";

        if (!(std::cin >> bet)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input.\n";
            continue;
        }

        if (bet <= 0 || !player.hasSufficientFunds(bet)) {
            std::cout << "Invalid bet amount.\n";
            continue;
        }

        return bet;
    }
}

template <typename T>
T Blackjack::getValidatedInput(T min, T max) const {
    T value;

    while (true) {
        if (std::cin >> value && value >= min && value <= max) {
            return value;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Try again: ";
    }
}
