// Source file for Horse Racing minigame/Horse Racing game class.
// Related Files: HorseRace.h, Player.h
// Date Created: 3/29/2026
// Last Edited: 4/4/2026

#include "HorseRace.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>

HorseRacing::HorseRacing()
    : m_rng(std::random_device{}()) {
    initializeHorses();
}

void HorseRacing::play(Player& player) {
    showLoadedBanner();

    while (true) {
        showMenu();
        const int choice = getMainMenuChoice();

        if (choice == 2) {
            std::cout << "\nReturning to game selection...\n";
            return;
        }

        const double betAmount = getBetAmount(player);

        std::cout << "\nChoose your horse:\n";
        showHorseList();
        const int horseChoice = getHorseChoice();

        if (!player.placeBet(betAmount)) {
            std::cout << "\nBet failed. You do not have enough funds.\n";
            continue;
        }

        runRace();
        const std::vector<Horse> results = getRaceResults();

        std::cout << "\nThe race is underway...\n";
        std::cout << "And the horses are crossing the finish line...\n";

        displayTopThree(results);

        const Horse& winningHorse = results[0];
        const Horse& selectedHorse = m_horses[horseChoice - 1];

        if (winningHorse.color == selectedHorse.color) {
            const double payout = betAmount * selectedHorse.payoutMultiplier;
            player.addWinnings(payout);

            std::cout << "\nYou picked the winning horse: " << selectedHorse.color << "!\n";
            std::cout << "Payout multiplier: x" << std::fixed << std::setprecision(2)
                << selectedHorse.payoutMultiplier << '\n';
            std::cout << "You won $" << payout << "!\n";
        }
        else {
            std::cout << "\nYour horse did not win.\n";
            std::cout << "You lost $" << std::fixed << std::setprecision(2) << betAmount << ".\n";
        }

        std::cout << "Current balance: $" << std::fixed << std::setprecision(2)
            << player.getBalance() << "\n\n";
    }
}

void HorseRacing::initializeHorses() {
    // Lower multiplier = more likely
    // Higher multiplier = less likely
    // Max multiplier is 2.00 so player can never receive more than 2x bet
    m_horses = { {
        {"Red",    1.10, 18.0, 0.0},
        {"Blue",   1.20, 16.0, 0.0},
        {"Green",  1.30, 14.0, 0.0},
        {"Yellow", 1.40, 12.0, 0.0},
        {"Black",  1.50, 10.0, 0.0},
        {"White",  1.65,  8.0, 0.0},
        {"Purple", 1.80,  6.0, 0.0},
        {"Orange", 2.00,  4.0, 0.0}
    } };
}

void HorseRacing::showLoadedBanner() const {
    std::cout << "=====================================\n";
    std::cout << "         HORSE RACING LOADED         \n";
    std::cout << "=====================================\n";
    std::cout << "Welcome to the text-only horse track.\n";
    std::cout << "Pick a horse color, place a bet, and\n";
    std::cout << "see if your horse finishes first.\n\n";
}

void HorseRacing::showMenu() const {
    std::cout << "1. Start Game\n";
    std::cout << "2. Return to Game Selection\n";
}

void HorseRacing::showHorseList() const {
    std::cout << std::fixed << std::setprecision(2);

    for (std::size_t index = 0; index < m_horses.size(); ++index) {
        std::cout << (index + 1) << ". "
            << std::setw(7) << std::left << m_horses[index].color
            << " | Payout: x" << m_horses[index].payoutMultiplier
            << '\n';
    }
}

int HorseRacing::getMainMenuChoice() const {
    int choice = 0;

    while (true) {
        std::cout << "\nEnter choice: ";
        if (std::cin >> choice && (choice == 1 || choice == 2)) {
            clearInput();
            return choice;
        }

        std::cout << "Invalid choice. Please enter 1 or 2.\n";
        std::cin.clear();
        clearInput();
    }
}

double HorseRacing::getBetAmount(const Player& player) const {
    double amount = 0.0;

    while (true) {
        std::cout << "\nYour balance: $" << std::fixed << std::setprecision(2)
            << player.getBalance() << '\n';
        std::cout << "Enter bet amount: $";

        if (!(std::cin >> amount)) {
            std::cout << "Invalid amount. Please enter a number.\n";
            std::cin.clear();
            clearInput();
            continue;
        }

        clearInput();

        if (amount <= 0.0) {
            std::cout << "Bet must be greater than 0.\n";
            continue;
        }

        if (!player.hasSufficientFunds(amount)) {
            std::cout << "You do not have enough balance for that bet.\n";
            continue;
        }

        return amount;
    }
}

int HorseRacing::getHorseChoice() const {
    int choice = 0;

    while (true) {
        std::cout << "\nPick a horse (1-" << m_horses.size() << "): ";
        if (std::cin >> choice &&
            choice >= 1 &&
            choice <= static_cast<int>(m_horses.size())) {
            clearInput();
            return choice;
        }

        std::cout << "Invalid choice. Please select a horse number from 1 to "
            << m_horses.size() << ".\n";
        std::cin.clear();
        clearInput();
    }
}

void HorseRacing::runRace() {
    std::uniform_real_distribution<double> randomness(0.0, 100.0);

    for (Horse& horse : m_horses) {
        // Single simple calculation:
        // stronger horses tend to do better, but randomness can upset the race
        horse.finishScore = horse.strengthWeight + randomness(m_rng);
    }
}

std::vector<HorseRacing::Horse> HorseRacing::getRaceResults() const {
    std::vector<Horse> results(m_horses.begin(), m_horses.end());

    std::sort(results.begin(), results.end(),
        [](const Horse& left, const Horse& right) {
            return left.finishScore > right.finishScore;
        });

    return results;
}

void HorseRacing::displayTopThree(const std::vector<Horse>& results) const {
    std::cout << "\n===== TOP 3 FINISHERS =====\n";
    for (int index = 0; index < 3; ++index) {
        std::cout << (index + 1) << ". "
            << results[index].color
            << " (score: " << std::fixed << std::setprecision(2)
            << results[index].finishScore << ")\n";
    }
    std::cout << "===========================\n";
}

void HorseRacing::clearInput() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}