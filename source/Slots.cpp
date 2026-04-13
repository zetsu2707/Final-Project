// Description: Implementation file for the Slots game class.
// Related Files:
// Date Created: 3/29/2026
// Last Edited: 3/29/2026

#include"Slots.h"

// Description: Implementation file for the Slots game class.
// Related Files: Slots.h, Player.h, SlotsUI.h
// Date Created: 3/29/2026
// Last Edited: 4/13/2026

#include "Slots.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <numeric>

// ─────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────

Slots::Slots() {
    std::random_device rd;
    m_rng.seed(rd());
    initializeSymbols();
}

// ─────────────────────────────────────────
// Symbol Setup
// ─────────────────────────────────────────

void Slots::initializeSymbols() {
    // display, weight (higher = more common), multiplier
    m_symbols = {
        { "Cherry",     40, 2.0  },
        { "Lemon",      30, 3.0  },
        { "Orange",     20, 5.0  },
        { "Grapes",     15, 8.0  },
        { "Watermelon", 10, 15.0 },
        { "Star",        5, 50.0 },
    };
}

// ─────────────────────────────────────────
// Public UI interface
// ─────────────────────────────────────────

std::vector<Slots::Symbol> Slots::spinReels() {
    int totalWeight = 0;
    for (const auto& s : m_symbols) totalWeight += s.weight;

    std::uniform_int_distribution<int> dist(0, totalWeight - 1);

    std::vector<Symbol> result;
    for (int reel = 0; reel < 3; ++reel) {
        int roll       = dist(m_rng);
        int cumulative = 0;
        for (const auto& s : m_symbols) {
            cumulative += s.weight;
            if (roll < cumulative) {
                result.push_back(s);
                break;
            }
        }
    }
    return result;
}

double Slots::evaluate(const std::vector<Symbol>& result) const {
    if (result.size() < 3) return 0.0;

    if (result[0].display == result[1].display &&
        result[1].display == result[2].display)
        return result[0].multiplier;

    if (result[0].display == result[1].display ||
        result[1].display == result[2].display ||
        result[0].display == result[2].display)
        return 0.5;

    return 0.0;
}

const std::vector<Slots::Symbol>& Slots::getSymbols() const {
    return m_symbols;
}

// ─────────────────────────────────────────
// Display (console mode)
// ─────────────────────────────────────────

void Slots::showBanner() const {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════╗\n";
    std::cout << "║         FRUIT SLOT MACHINE       ║\n";
    std::cout << "╚══════════════════════════════════╝\n\n";
}

void Slots::showReels(const std::vector<Symbol>& result) const {
    std::cout << "\n┌─────────┬─────────┬─────────┐\n";
    std::cout << "│";
    for (const auto& s : result)
        std::cout << "  " << s.display << "  │";
    std::cout << "\n└─────────┴─────────┴─────────┘\n\n";
}

// ─────────────────────────────────────────
// Input Helpers (console mode)
// ─────────────────────────────────────────

double Slots::getBet(const Player& player) const {
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
T Slots::getValidatedInput(T min, T max) const {
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

// ─────────────────────────────────────────
// play() — console fallback
// ─────────────────────────────────────────

void Slots::play(Player& player) {
    showBanner();

    while (true) {
        std::cout << "Balance: $" << std::fixed << std::setprecision(2)
                  << player.getBalance() << "\n";
        std::cout << "1. Spin\n2. Quit\nChoice: ";
        int choice = getValidatedInput<int>(1, 2);
        if (choice == 2) break;

        if (player.getBalance() <= 0.0) {
            std::cout << "You're out of funds!\n";
            break;
        }

        double bet = getBet(player);
        player.placeBet(bet);

        std::cout << "\nSpinning...\n";
        auto result = spinReels();
        showReels(result);

        double multiplier = evaluate(result);
        if (multiplier == 0.0) {
            std::cout << "No match. You lose $" << std::fixed
                      << std::setprecision(2) << bet << ".\n\n";
        } else if (multiplier == 0.5) {
            double winnings = bet * 0.5;
            std::cout << "Two of a kind! You get back $" << winnings << ".\n\n";
            player.addWinnings(winnings);
        } else {
            double winnings = bet * multiplier;
            std::cout << "THREE OF A KIND! " << result[0].display
                      << " x" << multiplier << " — You win $" << winnings << "!\n\n";
            player.addWinnings(winnings);
        }

        std::cout << "1. Spin again\n2. View payouts\n3. Quit\nChoice: ";
        int next = getValidatedInput<int>(1, 3);
        if (next == 3) break;
        if (next == 2) {
            std::cout << "\n── Payout Table ──\n";
            for (const auto& s : m_symbols)
                std::cout << s.display << " x3 = " << s.multiplier
                          << "x  |  x2 = 0.5x\n";
            std::cout << "\n";
        }
    }
}
