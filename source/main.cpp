// Description: Implementation file for the CasinoGame class.
// Related Files:
// Date Created: 3/29/2026
// Last Edited: 4/4/2026
#include <iostream>
#include "Player.h"

int main() {
    Player player1("Yoltic Alvarez", 200.0);
    Player player2("Gabriel Carmona", 1000000.0);

    int choice;
    bool running = true;

    while (running) {
        std::cout << "\n=== Player Driver ===\n";
        std::cout << "1. Save Player 1\n";
        std::cout << "2. Save Player 2\n";
        std::cout << "3. Load Player 1\n";
        std::cout << "4. Load Player 2\n";
        std::cout << "5. Display Player 1\n";
        std::cout << "6. Display Player 2\n";
        std::cout << "0. Exit\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                if (player1.saveToFile("player1.txt"))
                    std::cout << "Player 1 saved successfully\n";
                else
                    std::cout << "Failed to save Player 1\n";
                break;

            case 2:
                if (player2.saveToFile("player2.txt"))
                    std::cout << "Player 2 saved successfully\n";
                else
                    std::cout << "Failed to save Player 2\n";
                break;

            case 3:
                if (player1.loadFromFile("player1.txt"))
                    std::cout << "Player 1 loaded successfully\n";
                else
                    std::cout << "Failed to load Player 1\n";
                break;

            case 4:
                if (player2.loadFromFile("player2.txt"))
                    std::cout << "Player 2 loaded successfully\n";
                else
                    std::cout << "Failed to load Player 2\n";
                break;

            case 5:
                std::cout << "Name: " << player1.getName() << '\n';
                std::cout << "Balance: $" << player1.getBalance() << '\n';
                break;

            case 6:
                std::cout << "Name: " << player2.getName() << '\n';
                std::cout << "Balance: $" << player2.getBalance() << '\n';
                break;

            case 0:
                running = false;
                break;

            default:
                std::cout << "Invalid choice\n";
                break;
        }
    }

    return 0;
}