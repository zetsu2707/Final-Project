// Header file for Poker UI/controller class.
// Related Files: PokerUI.cpp, Poker.h, CasinoGameUI.cpp, Audio.h
// Date Created: 4/13/2026
// Last Edited: 4/19/2026

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>

#include "Poker.h"
#include "Player.h"

class AudioManager;

class PokerUI
{
public:
    PokerUI(Player& player, AudioManager& audio);
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
        void setLabel(const std::string& label);
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

    struct SeatAnchor
    {
        float centerXFactor;
        float centerYFactor;
        bool labelAbove;
    };

    void refreshTexts();
    void updateButtonStates();
    void updateWagerFieldMode();
    void playRoundResultSound();
    bool isPlayerWinningResult(Poker::RoundResult result) const;
    void handleBackspace();
    void handleTextEntered(const sf::Event& event);
    void handleMouseClick(const sf::Event& event, sf::RenderWindow& window);

    void draw(sf::RenderWindow& window);
    void drawTableScene(sf::RenderWindow& window);
    void drawOpponents(sf::RenderWindow& window) const;
    void drawOpponentSeat(sf::RenderWindow& window,
        const Poker::OpponentView& opponent,
        sf::Vector2f center,
        bool labelAbove) const;
    void drawHand(sf::RenderWindow& window,
        const std::vector<Poker::CardView>& cards,
        float centerX,
        float centerY,
        bool hideCards) const;
    void drawCommunityRow(sf::RenderWindow& window,
        const std::vector<Poker::CardView>& cards,
        float centerX,
        float centerY) const;
    void drawCard(sf::RenderWindow& window,
        const Poker::CardView& card,
        sf::Vector2f position,
        bool hidden) const;
    void drawEmptyCardSlot(sf::RenderWindow& window, sf::Vector2f position) const;

    const sf::Texture* getCardTexture(const std::string& path) const;
    std::string getCardTexturePath(const Poker::CardView& card) const;
    double getParsedWager() const;
    static std::string toLower(const std::string& text);

    Player& m_player;
    AudioManager& m_audio;
    Poker m_poker;

    bool m_running;
    bool m_wagerFieldActive;

    sf::Font m_font;

    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

    sf::Texture m_tableTexture;
    sf::Sprite m_tableSprite;

    sf::Text m_title;
    sf::Text m_status;
    sf::Text m_balanceText;
    sf::Text m_potText;
    sf::Text m_phaseText;
    sf::Text m_currentBetText;
    sf::Text m_playerContributionText;
    sf::Text m_showdownText;
    sf::Text m_communityLabel;
    sf::Text m_playerLabel;

    InputField m_wagerInput;

    Button m_dealButton;
    Button m_foldButton;
    Button m_checkCallButton;
    Button m_raiseButton;
    Button m_backButton;

    mutable std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_cardTextures;
    std::vector<SeatAnchor> m_opponentSeats;
    sf::Vector2f m_cardSize;
};
