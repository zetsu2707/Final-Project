// Header file for CasinoGameUI game selection/controller class.
// Related Files: CasinoGameUI.cpp, Player.h, Blackjack.h, HorseRace.h, Roulette.h, Poker.h
// Date Created: 4/7/2026
// Last Edited: 4/10/2026

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "Player.h"


class CasinoGameUI
{
public:
    explicit CasinoGameUI(Player& player);
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
    void refreshPlayerStats();
    void handleBackspace();
    void handleTextEntered(const sf::Event& event);
    void handleMouseClick(const sf::Event& event, sf::RenderWindow& window);
    void saveGame();
    void clearSaveInput();

    Player& m_player;
    bool m_running;
    bool m_saveModeActive;
    bool m_saveFieldActive;

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

    ImageCard m_horseRaceCard;
    ImageCard m_blackjackCard;
    ImageCard m_rouletteCard;
    ImageCard m_pokerCard;
    ImageCard m_slotsCard;

    Button m_saveGameButton;
    Button m_returnButton;
    Button m_confirmSaveButton;

    InputField m_saveFilenameInput;
};