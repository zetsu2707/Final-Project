// Description: Implementation file for the CasinoGame menu class.
// Related Files: CasinoGame.h, Player.h, Blackjack.h, HorseRace.h, Roulette.h, Poker.h
// Date Created: 3/29/2026
// Last Edited: 4/5/2026

#include "CasinoGame.h"

#include "Blackjack.h"
#include "HorseRace.h"
#include "Roulette.h"
#include "Poker.h"

#include <iostream>
#include <limits>
#include <string>

int CasinoGame::getValidatedInt(const std::string& prompt, int minValue, int maxValue) const {
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

void CasinoGame::displayPlayerStats(const Player& player) const {
    std::cout << "Player: " << player.getName() << '\n';
    std::cout << "Balance: $" << player.getBalance() << '\n';
    std::cout << "Total Betted: $" << player.getTotalBetted() << '\n';
    std::cout << "Total Won: $" << player.getTotalWon() << '\n';
    std::cout << "Games Played: " << player.getGamesPlayed() << '\n';
    std::cout << "Games Won: " << player.getGamesWon() << '\n';
    std::cout << "Net Profit: $" << player.getNetProfit() << '\n';
}

void CasinoGame::displayMenu(const Player& player) const {
    std::cout << "\n=========== GAME SELECTION ===========\n";
    displayPlayerStats(player);
    std::cout << "--------------------------------------\n";
    std::cout << "1. Horse Race\n";
    std::cout << "2. Blackjack\n";
    std::cout << "3. Roulette\n";
    std::cout << "4. Poker\n";
    std::cout << "5. Save Game\n";
    std::cout << "6. Return to Main Menu\n";
    std::cout << "======================================\n";
}

void CasinoGame::saveGame(Player& player) const {
    std::string filename;

    std::cout << "Enter save filename: ";
    std::getline(std::cin, filename);

    if (filename.empty()) {
        std::cout << "Save cancelled. Filename cannot be empty.\n";
        return;
    }

    if (player.saveToFile(filename)) {
        std::cout << "Game saved successfully to \"" << filename << "\".\n";
    } else {
        std::cout << "Failed to save game.\n";
    }
}

void CasinoGame::run(Player& player) {
    bool running = true;

    while (running) {
        displayMenu(player);

        int choice = getValidatedInt("Choose a game or option: ", 1, 6);

        switch (choice) {
            case 1: {
                HorseRacing horseRace;
                horseRace.play(player);
                break;
            }

            case 2: {
                Blackjack blackjack;
                blackjack.play(player);
                break;
            }

            case 3: {
                Roulette roulette;
                roulette.play(player);
                break;
            }

            case 4: {
                Poker poker;
                poker.play(player);
                break;
            }

            case 5:
                saveGame(player);
                break;

            case 6:
                std::cout << "Returning to main menu.\n";
                running = false;
                break;

            default:
                std::cout << "Invalid choice.\n";
                break;
        }
    }
}