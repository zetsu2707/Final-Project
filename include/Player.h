// Header file for Player class.
// Related Files: HorseRacing.cpp, Player.h
// Date Created: 3/29/2026
// Last Edited: 4/24/2026

#pragma once
#include <map>
#include <string>
#include <string_view>

class Player {
public:
    struct GameStat {
        int    played      = 0;
        int    won         = 0;
        double netEarnings = 0.0;
    };

    Player(const std::string_view name, double startingBalance = 200.0);

    static std::string getSlotFilename(int slot); // "slot_N.sav", slot 1-10

    // Saving
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);

    // Core Gameplay
    bool placeBet(double amount);
    void addWinnings(double amount);
    bool hasSufficientFunds(double amount) const;

    // Per-game stat recording
    void recordGameResult(const std::string& gameName, bool won);
    void updateGameEarnings(const std::string& gameName, double netEarnings);

    // Getters
    std::string getName()        const;
    double      getBalance()     const;
    double      getTotalBetted() const;
    double      getTotalWon()    const;
    int         getGamesPlayed() const;
    int         getGamesWon()    const;
    int         getGamesLost()   const;
    double      getNetProfit()   const;

    const std::map<std::string, GameStat>& getGameStats()        const;
    std::string getHighestEarningGame() const;
    std::string getMostWinsGame()       const;

private:
    std::string m_name;
    double      m_balance;
    double      m_totalBetted;
    double      m_totalWon;
    int         m_gamesPlayed;
    int         m_gamesWon;

    std::map<std::string, GameStat> m_gameStats;
};
