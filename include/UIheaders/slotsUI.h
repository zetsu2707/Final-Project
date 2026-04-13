// Header file for Slots UI/controller class.
// Related Files: SlotsUI.cpp, Slots.h, Player.h, Audio.h, CasinoGameUI.cpp
// Date Created: 4/13/2026
// Last Edited: 4/13/2026

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "Slots.h"
#include "Player.h"

class AudioManager;

class SlotsUI
{
public:
    SlotsUI(Player& player, AudioManager& audio);
    void run(sf::RenderWindow& window);

private:
    // ── Inner UI helpers ────────────────────────────────────────────────────
    struct Button
    {
        sf::RectangleShape box;
        sf::Text           text;
        bool               enabled = true;

        Button(const sf::Font& font,
               const std::string& label,
               sf::Vector2f position,
               sf::Vector2f size);

        bool contains(sf::Vector2f point) const;
        void update(const sf::RenderWindow& window);
        bool isClicked(const sf::Event& event, const sf::RenderWindow& window) const;
        void draw(sf::RenderWindow& window) const;
    };

    struct InputField
    {
        sf::RectangleShape box;
        sf::Text           text;
        std::string        value;
        std::string        placeholder;

        InputField(const sf::Font& font,
                   const std::string& placeholderText,
                   sf::Vector2f position,
                   sf::Vector2f size);

        bool contains(sf::Vector2f point) const;
        void setActive(bool active);
        void syncText();
        void draw(sf::RenderWindow& window) const;
    };

    // ── State ────────────────────────────────────────────────────────────────
    enum class SlotState { Idle, Spinning, Result };

    // ── Methods ─────────────────────────────────────────────────────────────
    void handleTextEntered(const sf::Event& event);
    void handleMouseClick(const sf::Event& event, sf::RenderWindow& window);
    void handleBackspace();

    void doSpin();
    void refreshTexts();
    void updateButtonStates();
    double getParsedBet() const;

    void draw(sf::RenderWindow& window);
    void drawReels(sf::RenderWindow& window) const;
    void drawPayoutTable(sf::RenderWindow& window) const;
    void drawResultBanner(sf::RenderWindow& window) const;

    // ── Members ──────────────────────────────────────────────────────────────
    Player&       m_player;
    AudioManager& m_audio;
    Slots         m_slots;

    bool m_running;
    bool m_betFieldActive;
    bool m_showPayouts;

    SlotState                m_state;
    std::vector<Slots::Symbol> m_lastResult;   // exposed via friend / accessor — see note below
    double                   m_lastMultiplier;
    double                   m_lastBet;
    std::string              m_resultMessage;
    sf::Color                m_resultColor;

    // Reel spin animation
    float m_spinTimer;
    static constexpr float SpinDuration = 0.9f; // seconds of "spinning" visual

    sf::Font m_font;

    sf::Texture m_backgroundTexture;
    sf::Sprite  m_backgroundSprite;

    sf::Text m_title;
    sf::Text m_balanceText;
    sf::Text m_statusText;

    InputField m_betInput;

    Button m_spinButton;
    Button m_backButton;
    Button m_payoutsButton;
};
