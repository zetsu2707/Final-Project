// Header file for CasinoGameUI game selection/controller class.
// Related Files: CasinoGameUI.cpp, Player.h, Blackjack.h, HorseRace.h, Roulette.h, Poker.h, Audio.h
// Date Created: 4/7/2026
// Last Edited: 4/20/2026

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "Player.h"

class AudioManager;

class CasinoGameUI
{
public:
    CasinoGameUI(Player& player, AudioManager& audio);
    void run(sf::RenderWindow& window);

private:
    struct Button
    {
        sf::RectangleShape box;
        sf::Text text;

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
        sf::Text text;
        std::string value;
        std::string placeholder;

        InputField(const sf::Font& font,
            const std::string& placeholderText,
            sf::Vector2f position,
            sf::Vector2f size);

        bool contains(sf::Vector2f point) const;
        void setActive(bool active);
        void syncText();
        void draw(sf::RenderWindow& window) const;
    };

    struct ImageCard
    {
        sf::Texture texture;
        sf::Sprite sprite;
        sf::RectangleShape hoverOutline;

        ImageCard() = default;
        ImageCard(const std::string& texturePath, sf::Vector2f position, sf::Vector2f size);

        bool contains(sf::Vector2f point) const;
        void update(const sf::RenderWindow& window);
        bool isClicked(const sf::Event& event, const sf::RenderWindow& window) const;
        void draw(sf::RenderWindow& window) const;
    };

    void draw(sf::RenderWindow& window);
    void drawEndScreen(sf::RenderWindow& window);   // NEW: win/lose screen
    void refreshPlayerStats();
    void checkEndCondition();                        // NEW: check $0 or $5000
    void handleBackspace();
    void handleTextEntered(const sf::Event& event);
    void handleMouseClick(const sf::Event& event, sf::RenderWindow& window);
    void saveGame();
    void clearSaveInput();

    Player& m_player;
    AudioManager& m_audio;

    bool m_running;
    bool m_saveModeActive;
    bool m_saveFieldActive;

    // End-screen state
    bool        m_showEndScreen = false;
    bool        m_playerWon = false;   // true = win ($5000), false = lose ($0)

    sf::Font m_font;

    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

    sf::Texture m_namePanelTexture;
    sf::Sprite m_namePanelSprite;

    sf::Texture m_balancePanelTexture;
    sf::Sprite m_balancePanelSprite;

    sf::Text m_title;
    sf::Text m_status;
    sf::Text m_playerNameText;
    sf::Text m_balanceText;

    // End-screen texts
    sf::Text m_endTitle;
    sf::Text m_endSubtitle;
    sf::Text m_endStats;

    ImageCard m_horseRaceCard;
    ImageCard m_blackjackCard;
    ImageCard m_rouletteCard;
    ImageCard m_pokerCard;
    ImageCard m_slotsCard;

    Button m_saveGameButton;
    Button m_returnButton;
    Button m_confirmSaveButton;
    Button m_endMenuButton;     // NEW: "Return to Menu" on end screen

    InputField m_saveFilenameInput;
};