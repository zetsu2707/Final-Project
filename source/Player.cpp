// Description: Implementation file for the Player class.
// Related Files:
// Date Created: 3/29/2026
// Last Edited: 4/4/2026

#include <fstream>
#include "Player.h"

Player::Player(const std::string_view name, double startingBalance)
    : m_name(name), m_balance(startingBalance), 
      m_totalBetted(0.0), m_totalWon(0.0),
      m_gamesPlayed(0), m_gamesWon(0) {}

std::string Player::getName() const;
double Player::getBalance() const;
double Player::getTotalBetted() const;
double Player::getTotalWon() const;

bool Player::placeBet(double amount);
void Player::addWinnings(double amount);
bool Player::hasSufficientFunds(double amount) const;

bool Player::saveToFile(const std::string& filepath) const {
    std::ofstream outFile("data/" + filepath);
    
    if (!outFile) {
        return false;
    }

    outFile << m_name << '\n';
    outFile << m_balance << '\n';
    outFile << m_totalBetted << '\n';
    outFile << m_totalWon << '\n';
    outFile << m_gamesPlayed << '\n';
    outFile << m_gamesWon << '\n';
    return true;
}

bool Player::loadFromFile(const std::string& filepath) {
    std::ifstream inFile("data/" + filepath);
    
    if (!inFile) {
        return false;
    }

    std::getline(inFile, m_name);
    inFile >> m_balance;
    inFile >> m_totalBetted;
    inFile >> m_totalWon;
    inFile >> m_gamesPlayed;
    inFile >> m_gamesWon;

    if (inFile.fail()) {
        return false;
    }

    return true;
}
