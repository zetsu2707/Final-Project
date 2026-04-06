// Description: Main menu driver for the casino game project.
// Related Files: Player.h, CasinoGame.h
// Date Created: 3/29/2026
// Last Edited: 4/5/2026

#include "Player.h"
#include "CasinoGame.h"
#include <iostream>
#include <limits>
#include <string>

namespace {
    int getValidatedInt(const std::string& prompt, int minValue, int maxValue) {
        int value = 0;

        while (true) {
            std::cout << prompt;
            std::cin >> value;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter a number.\n";
                continue;
            }

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (value < minValue || value > maxValue) {
                std::cout << "Please enter a choice between "
                          << minValue << " and " << maxValue << ".\n";
                continue;
            }

            return value;
        }
    }

    double getValidatedDouble(const std::string& prompt, double minValue) {
        double value = 0.0;

        while (true) {
            std::cout << prompt;
            std::cin >> value;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter a valid number.\n";
                continue;
            }

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (value < minValue) {
                std::cout << "Value must be at least " << minValue << ".\n";
                continue;
            }

            return value;
        }
    }

    std::string getNonEmptyLine(const std::string& prompt) {
        std::string input;

        while (true) {
            std::cout << prompt;
            std::getline(std::cin, input);

            if (input.empty()) {
                std::cout << "Input cannot be empty.\n";
                continue;
            }

            return input;
        }
    }

    void displayInstructions() {
        std::cout << "\n========== CASINO INSTRUCTIONS ==========\n";

        std::cout << "\nBLACKJACK:\n";
        std::cout << "- Goal: get as close to 21 as possible without going over.\n";
        std::cout << "- Number cards are worth face value.\n";
        std::cout << "- Face cards are worth 10.\n";
        std::cout << "- Aces are worth 1 or 11.\n";
        std::cout << "- Beat the dealer by having a higher total than the dealer,\n";
        std::cout << "  or by the dealer busting.\n";

        std::cout << "\nROULETTE:\n";
        std::cout << "- Players place bets on where the ball will land.\n";
        std::cout << "- Common bets include red/black, odd/even, ranges,\n";
        std::cout << "  or exact numbers.\n";
        std::cout << "- Matching bets win based on the type of wager.\n";

        std::cout << "\nSLOTS:\n";
        std::cout << "- Spin the reels and try to match symbols.\n";
        std::cout << "- Matching combinations pay different amounts.\n";

        std::cout << "\nPOKER:\n";
        std::cout << "- Poker compares hand rankings.\n";
        std::cout << "- Better hand wins.\n";
        std::cout << "- Common hands include pair, two pair, straight,\n";
        std::cout << "  flush, and full house.\n";

        std::cout << "\nHORSE RACES:\n";
        std::cout << "- Pick a horse and place a bet before the race starts.\n";
        std::cout << "- If your horse wins, you get paid based on the odds.\n";

        std::cout << "\nGENERAL:\n";
        std::cout << "- Manage your balance carefully.\n";
        std::cout << "- Save your progress so you can load it later.\n";

        std::cout << "=========================================\n";
    }

    void displayStatisticsPlaceholder() {
        std::cout << "\n========== STATISTICS ==========\n";
        std::cout << "Statistics are still a work in progress.\n";
        std::cout << "This option is a placeholder for now.\n";
        std::cout << "================================\n";
    }

    void displayPlayerSummary(const Player& player) {
        std::cout << "\n========== PLAYER INFO ==========\n";
        std::cout << "Name: " << player.getName() << '\n';
        std::cout << "Balance: $" << player.getBalance() << '\n';
        std::cout << "Total Betted: $" << player.getTotalBetted() << '\n';
        std::cout << "Total Won: $" << player.getTotalWon() << '\n';
        std::cout << "Games Played: " << player.getGamesPlayed() << '\n';
        std::cout << "Games Won: " << player.getGamesWon() << '\n';
        std::cout << "Net Profit: $" << player.getNetProfit() << '\n';
        std::cout << "=================================\n";
    }

    void handleNewGame() {
        std::cout << "\n========== NEW GAME ==========\n";

        std::string name = getNonEmptyLine("Enter player name: ");
        double startingBalance = getValidatedDouble("Enter starting balance: $", 0.0);

        Player player(name, startingBalance);

        std::cout << "\nNew game started successfully.\n";
        displayPlayerSummary(player);

        CasinoGame casinoMenu;
        casinoMenu.run(player);
    }

    void handleLoadGame() {
        std::cout << "\n========== LOAD GAME ==========\n";

        std::string filename = getNonEmptyLine("Enter save filename: ");
        Player player("Unknown Player");

        if (player.loadFromFile(filename)) {
            std::cout << "Game loaded successfully.\n";
            displayPlayerSummary(player);

            CasinoGame casinoMenu;
            casinoMenu.run(player);
        } else {
            std::cout << "Failed to load save file.\n";
        }
    }
}

int main() {
    bool running = true;

    while (running) {
        std::cout << "\n=========== CASINO MAIN MENU ===========\n";
        std::cout << "1. New Game\n";
        std::cout << "2. Load Game\n";
        std::cout << "3. Statistics\n";
        std::cout << "4. Instructions\n";
        std::cout << "5. Exit\n";
        std::cout << "========================================\n";

        int choice = getValidatedInt("Choose an option: ", 1, 5);

        switch (choice) {
            case 1:
                handleNewGame();
                break;

            case 2:
                handleLoadGame();
                break;

            case 3:
                displayStatisticsPlaceholder();
                break;

            case 4:
                displayInstructions();
                break;

            case 5:
                std::cout << "Exiting game. Goodbye.\n";
                running = false;
                break;

            default:
                std::cout << "Invalid choice.\n";
                break;
        }
    }

    return 0;
}