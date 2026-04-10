// Source file for Horse Racing minigame/Horse Racing game class.
// Related Files: HorseRace.h, Player.h
// Date Created: 3/29/2026
// Last Edited: 4/10/2026

#include "HorseRace.h"

#include <algorithm>

HorseRacing::HorseRacing()
    : m_rng(std::random_device{}()) {
    initializeHorses();
    resetRaceData();
}

const std::array<HorseRacing::Horse, HorseRacing::HorseCount>& HorseRacing::getHorses() const {
    return m_horses;
}

const HorseRacing::RaceState& HorseRacing::getRaceState() const {
    return m_raceState;
}

void HorseRacing::initializeHorses() {
    m_horses = { {
        {"Red",    2.00, 12.5, 0.0, {}, 0.0},
        {"Blue",   2.00, 12.5, 0.0, {}, 0.0},
        {"Green",  2.00, 12.5, 0.0, {}, 0.0},
        {"Yellow", 2.00, 12.5, 0.0, {}, 0.0},
        {"Black",  2.00, 12.5, 0.0, {}, 0.0},
        {"White",  2.00, 12.5, 0.0, {}, 0.0},
        {"Purple", 2.00, 12.5, 0.0, {}, 0.0},
        {"Orange", 2.00, 12.5, 0.0, {}, 0.0}
    } };
}

void HorseRacing::resetRaceData() {
    m_raceState.started = false;
    m_raceState.finished = false;
    m_raceState.currentPhase = 0;

    for (Horse& horse : m_horses) {
        horse.finishScore = 0.0;
        horse.currentProgress = 0.0;
        horse.phaseScores.fill(0.0);
    }
}

void HorseRacing::beginRace() {
    resetRaceData();

    std::uniform_real_distribution<double> randomness(8.0, 28.0);

    for (Horse& horse : m_horses) {
        for (int phase = 0; phase < RacePhaseCount; ++phase) {
            const double phaseStrengthContribution = horse.strengthWeight * 0.35;
            horse.phaseScores[phase] = phaseStrengthContribution + randomness(m_rng);
        }
    }

    m_raceState.started = true;
}

bool HorseRacing::advanceRacePhase() {
    if (!m_raceState.started || m_raceState.finished) {
        return false;
    }

    if (m_raceState.currentPhase >= RacePhaseCount) {
        m_raceState.finished = true;
        return false;
    }

    for (Horse& horse : m_horses) {
        horse.currentProgress += horse.phaseScores[m_raceState.currentPhase];
    }

    ++m_raceState.currentPhase;

    if (m_raceState.currentPhase >= RacePhaseCount) {
        m_raceState.finished = true;

        for (Horse& horse : m_horses) {
            horse.finishScore = horse.currentProgress;
        }
    }

    return true;
}

void HorseRacing::runRace() {
    beginRace();

    while (!m_raceState.finished) {
        advanceRacePhase();
    }
}

std::vector<HorseRacing::Horse> HorseRacing::getRaceResults() const {
    std::vector<Horse> results(m_horses.begin(), m_horses.end());

    std::sort(results.begin(), results.end(),
        [](const Horse& left, const Horse& right) {
            return left.currentProgress > right.currentProgress;
        });

    return results;
}

double HorseRacing::getHorseProgress(int horseIndex) const {
    if (horseIndex < 0 || horseIndex >= HorseCount) {
        return 0.0;
    }

    return m_horses[horseIndex].currentProgress;
}

double HorseRacing::getLeaderProgress() const {
    double leaderProgress = 0.0;

    for (const Horse& horse : m_horses) {
        if (horse.currentProgress > leaderProgress) {
            leaderProgress = horse.currentProgress;
        }
    }

    return leaderProgress;
}

bool HorseRacing::isRaceStarted() const {
    return m_raceState.started;
}

bool HorseRacing::isRaceFinished() const {
    return m_raceState.finished;
}

bool HorseRacing::isValidHorseChoice(int horseChoice) const {
    return horseChoice >= 1 && horseChoice <= HorseCount;
}

bool HorseRacing::canPlaceBet(const Player& player, double betAmount) const {
    return betAmount > 0.0 && player.hasSufficientFunds(betAmount);
}

bool HorseRacing::resolveBet(Player& player, double betAmount, int horseChoice) {
    m_lastBetWon = false;
    m_lastPayout = 0.0;

    if (!m_raceState.finished) {
        return false;
    }

    if (!isValidHorseChoice(horseChoice)) {
        return false;
    }

    if (!canPlaceBet(player, betAmount)) {
        return false;
    }

    if (!player.placeBet(betAmount)) {
        return false;
    }

    const std::vector<Horse> results = getRaceResults();
    const Horse& winningHorse = results.front();
    const Horse& selectedHorse = m_horses[horseChoice - 1];

    if (winningHorse.color == selectedHorse.color) {
        const double payout = betAmount * selectedHorse.payoutMultiplier;
        player.addWinnings(payout);
        m_lastBetWon = true;
        m_lastPayout = payout;
    }

    return true;
}

bool HorseRacing::didPlayerWinLastBet() const {
    return m_lastBetWon;
}

double HorseRacing::getLastPayout() const {
    return m_lastPayout;
}