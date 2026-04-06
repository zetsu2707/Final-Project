// Roulette minigame implementation file.
// Related Files: Roulette.h, Player.h
// Date Created: 3/29/2026
// Last Edited: 04/05/2026

#include "Roulette.h"
#include <iostream>
#include <limits>

Roulette::Roulette() : m_rng(std::random_device{}()) {}

void Roulette::showBanner() const {
    std::cout << "============================\n";
    std::cout << "         ROULETTE           \n";
    std::cout << "============================\n";
}

int Roulette::spinWheel() {
    std::uniform_int_distribution<int> dist(0, 36);
    return dist(m_rng);
}

bool Roulette::isRed(int number) const {
    static std::vector<int> reds = {
        1,3,5,7,9,12,14,16,18,
        19,21,23,25,27,30,32,34,36
    };

    for (int r : reds) {
        if (r == number) return true;
    }
    return false;
}

double Roulette::getBetAmount(const Player& player) const {
    double bet;

    while (true) {
        std::cout << "Balance: $" << player.getBalance() << "\n";
        std::cout << "Enter bet amount: $";

        if (!(std::cin >> bet)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input.\n";
            continue;
        }

        if (bet <= 0 || !player.hasSufficientFunds(bet)) {
            std::cout << "Invalid bet.\n";
            continue;
        }

        return bet;
    }
}

int Roulette::getValidatedNumber(int min, int max) const {
    int value;

    while (true) {
        if (std::cin >> value && value >= min && value <= max) {
            return value;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Try again: ";
    }
}

void Roulette::play(Player& player) {
    showBanner();

    while (true) {
        std::cout << "\n--- Betting Options ---\n";
        std::cout << "1. Number (pays 35:1)\n";
        std::cout << "2. Red (pays 1:1)\n";
        std::cout << "3. Black (pays 1:1)\n";
        std::cout << "4. Even (pays 1:1)\n";
        std::cout << "5. Odd (pays 1:1)\n";
        std::cout << "0. Exit\n";
        std::cout << "Choice: ";

        int choice = getValidatedNumber(0, 5);

        if (choice == 0) break;

        double betAmount = getBetAmount(player);

        if (!player.placeBet(betAmount)) {
            std::cout << "Bet failed.\n";
            continue;
        }

        Bet bet;
        bet.amount = betAmount;

        if (choice == 1) {
            bet.type = "number";
            std::cout << "Pick a number (0-36): ";
            bet.number = getValidatedNumber(0, 36);
        }
        else if (choice == 2) bet.type = "red";
        else if (choice == 3) bet.type = "black";
        else if (choice == 4) bet.type = "even";
        else if (choice == 5) bet.type = "odd";

        int result = spinWheel();

        std::cout << "\nWheel landed on: " << result;
        if (result != 0) {
            std::cout << (isRed(result) ? " (Red)\n" : " (Black)\n");
        } else {
            std::cout << " (Green)\n";
        }

        bool win = false;

        if (bet.type == "number") {
            win = (bet.number == result);
            if (win) {
                double payout = bet.amount * 36;
                player.addWinnings(payout);
                std::cout << "You hit the number! Won $" << payout << "\n";
            }
        }
        else if (result != 0) {
            if (bet.type == "red") win = isRed(result);
            else if (bet.type == "black") win = !isRed(result);
            else if (bet.type == "even") win = (result % 2 == 0);
            else if (bet.type == "odd") win = (result % 2 == 1);

            if (win) {
                double payout = bet.amount * 2;
                player.addWinnings(payout);
                std::cout << "You win! Won $" << payout << "\n";
            }
        }

        if (!win) {
            std::cout << "You lose $" << bet.amount << "\n";
        }

        std::cout << "Current Balance: $" << player.getBalance() << "\n";

        std::cout << "\nPlay again? (1 = Yes, 0 = No): ";
        int again = getValidatedNumber(0, 1);
        if (again == 0) break;
    }
}
