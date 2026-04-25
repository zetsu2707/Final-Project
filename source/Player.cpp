// Description: Implementation file for the Player class.
// Date Created: 3/29/2026
// Last Edited: 4/24/2026

#include <fstream>
#include <limits>
#include "Player.h"

Player::Player(const std::string_view name, double startingBalance)
    : m_name(name), m_balance(startingBalance),
      m_totalBetted(0.0), m_totalWon(0.0),
      m_gamesPlayed(0), m_gamesWon(0) {}

std::string Player::getName()        const { return m_name; }
double      Player::getBalance()     const { return m_balance; }
double      Player::getTotalBetted() const { return m_totalBetted; }
double      Player::getTotalWon()    const { return m_totalWon; }
int         Player::getGamesPlayed() const { return m_gamesPlayed; }
int         Player::getGamesWon()    const { return m_gamesWon; }
int         Player::getGamesLost()   const { return m_gamesPlayed - m_gamesWon; }
double      Player::getNetProfit()   const { return m_totalWon - m_totalBetted; }

const std::map<std::string, Player::GameStat>& Player::getGameStats() const {
    return m_gameStats;
}

bool Player::placeBet(double amount) {
    if (amount <= 0.0 || !hasSufficientFunds(amount)) return false;
    m_balance    -= amount;
    m_totalBetted += amount;
    return true;
}

void Player::addWinnings(double amount) {
    if (amount > 0.0) {
        m_balance  += amount;
        m_totalWon += amount;
    }
}

std::string Player::getSlotFilename(int slot) {
    return "slot_" + std::to_string(slot) + ".sav";
}

bool Player::hasSufficientFunds(double amount) const {
    return amount > 0.0 && m_balance >= amount;
}

void Player::recordGameResult(const std::string& gameName, bool won) {
    GameStat& stat = m_gameStats[gameName];
    stat.played++;
    if (won) stat.won++;
    m_gamesPlayed++;
    if (won) m_gamesWon++;
}

void Player::updateGameEarnings(const std::string& gameName, double netEarnings) {
    m_gameStats[gameName].netEarnings += netEarnings;
}

std::string Player::getHighestEarningGame() const {
    if (m_gameStats.empty()) return "N/A";
    std::string best;
    double bestVal = std::numeric_limits<double>::lowest();
    for (const auto& [name, stat] : m_gameStats) {
        if (stat.netEarnings > bestVal) {
            bestVal = stat.netEarnings;
            best    = name;
        }
    }
    return best.empty() ? "N/A" : best;
}

std::string Player::getMostWinsGame() const {
    if (m_gameStats.empty()) return "N/A";
    std::string best;
    int bestWins = -1;
    for (const auto& [name, stat] : m_gameStats) {
        if (stat.won > bestWins) {
            bestWins = stat.won;
            best     = name;
        }
    }
    return best.empty() ? "N/A" : best;
}

// ── Save / Load ────────────────────────────────────────────────────────────────

bool Player::saveToFile(const std::string& filepath) const {
    std::ofstream out("data/" + filepath);
    if (!out) return false;

    out << m_name        << '\n';
    out << m_balance     << '\n';
    out << m_totalBetted << '\n';
    out << m_totalWon    << '\n';
    out << m_gamesPlayed << '\n';
    out << m_gamesWon    << '\n';

    out << m_gameStats.size() << '\n';
    for (const auto& [name, stat] : m_gameStats) {
        out << name             << '\n';
        out << stat.played      << '\n';
        out << stat.won         << '\n';
        out << stat.netEarnings << '\n';
    }

    return true;
}

bool Player::loadFromFile(const std::string& filepath) {
    std::ifstream in("data/" + filepath);
    if (!in) return false;

    std::getline(in, m_name);
    in >> m_balance >> m_totalBetted >> m_totalWon >> m_gamesPlayed >> m_gamesWon;
    if (in.fail()) return false;

    m_gameStats.clear();

    std::size_t count = 0;
    in >> count;
    in.ignore();

    for (std::size_t i = 0; i < count; ++i) {
        std::string name;
        std::getline(in, name);
        GameStat stat;
        in >> stat.played >> stat.won >> stat.netEarnings;
        in.ignore();
        if (in.fail()) break;
        m_gameStats[name] = stat;
    }

    return true;
}
