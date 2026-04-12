// Header file for Blackjack UI/controller class.
// Related Files: BlackjackUI.cpp, Blackjack.h, CasinoGameUI.cpp, Audio.h
// Date Created: 4/12/2026
// Last Edited: 4/12/2026

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>

#include "Blackjack.h"
#include "Player.h"

class AudioManager;

class BlackjackUI
{
public:
    BlackjackUI(Player& player, AudioManager& audio);
    void run(sf::RenderWindow& window);

private:
    struct Button
    {
        sf::RectangleShape box;
        sf::Text text;
        bool enabled;

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

    void refreshTexts();
    void updateButtonStates();
    void playRoundResultSound();
    bool isPlayerWinningResult(Blackjack::RoundResult result) const;
    void handleBackspace();
    void handleTextEntered(const sf::Event& event);
    void handleMouseClick(const sf::Event& event, sf::RenderWindow& window);

    void draw(sf::RenderWindow& window);
    void drawTableScene(sf::RenderWindow& window);
    void drawHand(sf::RenderWindow& window,
        const std::vector<Blackjack::CardView>& cards,
        float centerY,
        bool hideFirstCard) const;
    void drawCard(sf::RenderWindow& window,
        const Blackjack::CardView& card,
        sf::Vector2f position,
        bool hidden) const;

    const sf::Texture* getCardTexture(const std::string& path) const;
    std::string getCardTexturePath(const Blackjack::CardView& card) const;
    double getParsedBet() const;
    static std::string toLower(const std::string& text);

    Player& m_player;
    AudioManager& m_audio;
    Blackjack m_blackjack;

    bool m_running;
    bool m_betFieldActive;

    sf::Font m_font;

    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

    sf::Texture m_tableTexture;
    sf::Sprite m_tableSprite;

    sf::Text m_title;
    sf::Text m_status;
    sf::Text m_balanceText;
    sf::Text m_currentBetText;
    sf::Text m_dealerValueText;
    sf::Text m_playerValueText;

    InputField m_betInput;

    Button m_dealButton;
    Button m_hitButton;
    Button m_standButton;
    Button m_backButton;

    mutable std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_cardTextures;
    sf::Vector2f m_cardSize;
};