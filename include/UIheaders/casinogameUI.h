// Header file for CasinoGameUI game selection/controller class.
// Related Files: CasinoGameUI.cpp, Player.h, Audio.h
// Date Created: 4/7/2026
// Last Edited: 4/24/2026

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Player.h"

class AudioManager;

class CasinoGameUI
{
public:
    CasinoGameUI(Player& player, AudioManager& audio, int currentSlot = 0);
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

        void setLabel(const sf::Font& font, const std::string& label);
        bool contains(sf::Vector2f point) const;
        void update(const sf::RenderWindow& window);
        bool isClicked(const sf::Event& event, const sf::RenderWindow& window) const;
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
    void drawEndScreen(sf::RenderWindow& window);
    void drawSaveSlotMenu(sf::RenderWindow& window);
    void drawStatsPanel(sf::RenderWindow& window);

    void refreshPlayerStats();
    void refreshSaveSlotButtons();
    void buildStatsText();
    void checkEndCondition();
    void handleMouseClick(const sf::Event& event, sf::RenderWindow& window);

    Player&       m_player;
    AudioManager& m_audio;

    bool   m_running             = true;
    int    m_currentSlot         = 0;
    double m_horseRaceBalanceBefore = 0.0;

    bool m_showEndScreen  = false;
    bool m_playerWon      = false;
    bool m_showSaveMenu   = false;
    bool m_pendingReturn  = false;
    bool m_showStatsPanel = false;

    sf::Font m_font;

    sf::Texture m_backgroundTexture;
    sf::Sprite  m_backgroundSprite;

    sf::Texture m_namePanelTexture;
    sf::Sprite  m_namePanelSprite;

    sf::Texture m_balancePanelTexture;
    sf::Sprite  m_balancePanelSprite;

    sf::Text m_title;
    sf::Text m_status;
    sf::Text m_playerNameText;
    sf::Text m_balanceText;

    sf::Text m_endTitle;
    sf::Text m_endSubtitle;
    sf::Text m_endStats;

    sf::Text m_statsOverlayText;

    ImageCard m_horseRaceCard;
    ImageCard m_blackjackCard;
    ImageCard m_rouletteCard;
    ImageCard m_pokerCard;
    ImageCard m_slotsCard;

    Button m_saveGameButton;
    Button m_statsButton;
    Button m_returnButton;
    Button m_endMenuButton;
    Button m_saveCancelButton;
    Button m_saveNoSaveButton;
    Button m_statsPanelClose;

    std::vector<Button> m_saveSlotButtons;  // 10 save-slot buttons
};
