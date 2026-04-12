// Header file for HorseRace minigame UI class.
// Related Files: HorseRaceUI.cpp, HorseRace.h, Player.h, Audio.h
// Date Created: 4/10/2026
// Last Edited: 4/11/2026

#pragma once

#include "HorseRace.h"
#include "Player.h"

#include <SFML/Graphics.hpp>

#include <array>
#include <optional>
#include <string>

class AudioManager;

class HorseRaceUI {
public:
    HorseRaceUI(sf::RenderWindow& window, sf::Font& font, AudioManager& audio);

    void open(Player& player);
    void handleEvent(const sf::Event& event);
    void update(float deltaTime);
    void draw();

    bool wantsToLeave() const;
    void clearLeaveRequest();

private:
    enum class ScreenState {
        Betting,
        Racing,
        Results
    };

    static constexpr float BaseWidth = 1920.0f;
    static constexpr float BaseHeight = 1080.0f;

    static constexpr float TrackLeft = 230.0f;
    static constexpr float TrackRight = 1490.0f;
    static constexpr float TrackWidth = TrackRight - TrackLeft;

    static constexpr float TrackTop = 90.0f;
    static constexpr float LaneHeight = 95.0f;
    static constexpr float LaneGap = 10.0f;

    static constexpr float HorseDrawWidth = 120.0f;
    static constexpr float HorseDrawHeight = 58.0f;

    static constexpr float PhaseAnimationSeconds = 0.85f;
    static constexpr double MinimumBet = 5.0;

    sf::RenderWindow& m_window;
    sf::Font& m_font;
    AudioManager& m_audio;
    sf::View m_view;

    HorseRacing m_game;
    Player* m_player = nullptr;

    sf::Texture m_fieldTexture;
    std::optional<sf::Sprite> m_fieldSprite;
    bool m_hasFieldTexture = false;

    std::array<sf::Texture, HorseRacing::HorseCount> m_horseTextures;
    std::array<std::optional<sf::Sprite>, HorseRacing::HorseCount> m_horseSprites;
    std::array<bool, HorseRacing::HorseCount> m_hasHorseTexture{};

    std::array<float, HorseRacing::HorseCount> m_laneY{};
    std::array<double, HorseRacing::HorseCount> m_visualProgress{};
    std::array<double, HorseRacing::HorseCount> m_phaseStartProgress{};
    std::array<double, HorseRacing::HorseCount> m_phaseTargetProgress{};

    ScreenState m_screenState = ScreenState::Betting;
    int m_selectedHorse = 1;
    double m_betAmount = 10.0;

    std::string m_betInput;
    bool m_betFieldActive = false;
    bool m_betInputJustActivated = false;

    bool m_phaseAnimating = false;
    float m_phaseAnimationTimer = 0.0f;
    bool m_betSettled = false;
    bool m_wantsToLeave = false;

    double m_raceMaxProgress = 1.0;

    void updateView();
    void loadAssets();
    void setupLanes();
    void resetVisualState();

    void handleMousePressed(const sf::Vector2f& mousePosition);
    void handleBettingClick(const sf::Vector2f& mousePosition);
    void handleResultsClick(const sf::Vector2f& mousePosition);
    void handleTextEntered(char32_t unicode);

    void startRace();
    void beginNextPhaseAnimation();
    void updateRaceAnimation(float deltaTime);
    void settleBetIfNeeded();
    void syncHorseSprites();

    void setBetInputFromAmount(double amount);
    bool syncBetAmountFromInput();

    void drawBackground();
    void drawTrack();
    void drawHorseCards();
    void drawHorses();
    void drawBottomBar();
    void drawResultsPanel();

    sf::FloatRect getHorseCardBounds(int horseIndex) const;
    sf::FloatRect getBetInputBounds() const;
    sf::FloatRect getStartRaceButtonBounds() const;
    sf::FloatRect getBackButtonBounds() const;
    sf::FloatRect getPlayAgainButtonBounds() const;

    void drawButton(const sf::FloatRect& bounds, const std::string& label, bool highlighted = false);
    void drawText(const std::string& text,
        unsigned int size,
        const sf::Vector2f& position,
        const sf::Color& color = sf::Color::White);

    static sf::Color getHorseFallbackColor(const std::string& colorName);
};