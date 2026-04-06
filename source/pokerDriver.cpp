// Description: Driver to test Poker minigame
// Date Created: 4/5/2026
// Last Edited:  4/5/2026

#include "Poker.h"
#include "Player.h"
#include <iostream>

int main() {
    // Create a player with default $200 starting balance
    Player player("TestPlayer", 200.0);

    std::cout << "Welcome, " << player.getName() << "!\n";
    std::cout << "Starting balance: $" << player.getBalance() << "\n\n";

    // Launch the poker game
    Poker poker;
    poker.play(player);

    // Post-game stats
    std::cout << "\n══ Session Summary ══\n";
    std::cout << "Final balance:  $" << player.getBalance()  << "\n";
    std::cout << "Total betted:   $" << player.getTotalBetted() << "\n";
    std::cout << "Total won:      $" << player.getTotalWon()    << "\n";
    std::cout << "Net profit:     $" << player.getNetProfit()   << "\n";
    std::cout << "Games played:   "  << player.getGamesPlayed() << "\n";
    std::cout << "Games won:      "  << player.getGamesWon()    << "\n";

    return 0;
}