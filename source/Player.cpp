// Description: Implementation file for the Player class.
// Related Files:
// Date Created: 3/29/2026
// Last Edited: 3/29/2026

#include <fstream>
#include "Player.h"

Player::Player(const std::string_view name, double startingBalance)
    : m_name(name), m_balance(startingBalance),
      m_totalBetted(0.0), m_totalWon(0.0) {}

bool Player::saveToFile(const std::string& filepath) const {
    std::ofstream outFile("data/" + filepath);
    
    if (!outFile) {
        return false;
    }

    outFile << m_name << '\n';
    outFile << m_balance << '\n';
    outFile << m_totalBetted << '\n';
    outFile << m_totalWon << '\n';

    return true;
}