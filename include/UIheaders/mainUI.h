// Header file for MainUI start menu/controller class.
// Related Files: MainUI.cpp, Player.h, CasinoGameUI.h
// Date Created: 4/7/2026
// Last Edited: 4/24/2026

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Audio.h"
#include "Player.h"

class MainUI
{
public:
    MainUI(AudioManager& audio);
    void run(sf::RenderWindow& window);

private:
    AudioManager& m_audio;

    enum class Screen
    {
        GrandpaNote,
        Intro,
        MainMenu,
        Instructions,
        Statistics,
        NewGame,
        LoadGame
    };

    enum class ActiveField
    {
        None,
        NewGameName
    };

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

    struct InputField
    {
        sf::RectangleShape box;
        sf::Text text;
        std::string value;
        std::string placeholder;
        bool numericOnly;

        InputField(const sf::Font& font,
            const std::string& placeholderText,
            sf::Vector2f position,
            sf::Vector2f size,
            bool numeric = false);

        bool contains(sf::Vector2f point) const;
        void setActive(bool active);
        void syncText();
        void draw(sf::RenderWindow& window) const;
    };

    void drawGrandpaNote(sf::RenderWindow& window);
    void drawIntroScreen(sf::RenderWindow& window);
    void drawMainMenu(sf::RenderWindow& window);
    void drawInstructionsScreen(sf::RenderWindow& window);
    void drawStatisticsScreen(sf::RenderWindow& window);
    void drawNewGameScreen(sf::RenderWindow& window);
    void drawLoadGameScreen(sf::RenderWindow& window);

    void handleTextEntered(const sf::Event& event);
    void handleMouseClick(const sf::Event& event, sf::RenderWindow& window);
    void handleBackspace();
    void clearFormState();

    void refreshSlotButtons();
    void startNewGame(sf::RenderWindow& window);

    sf::Font m_font;

    sf::Texture m_backgroundTexture;
    sf::Sprite  m_backgroundSprite;
    sf::Texture m_logoTexture;
    sf::Sprite  m_logoSprite;

    sf::RectangleShape    m_noteBackground;
    sf::RectangleShape    m_notePaper;
    std::vector<sf::Text> m_noteLines;
    sf::Texture           m_noteTexture;
    sf::Sprite            m_noteSprite;
    sf::Text              m_noteContinuePrompt;
    sf::Clock             m_noteAlphaClock;
    float                 m_noteAlpha = 0.f;

    sf::Text  m_introTitle;
    sf::Text  m_introSubtitle;
    sf::Text  m_introPrompt;
    sf::Clock m_introClock;
    float     m_introAlpha = 0.f;

    sf::Text m_status;
    std::vector<sf::Text> m_instructionLines;

    sf::Text m_statisticsTitle;
    sf::Text m_statisticsBody;
    sf::Text m_newGameTitle;
    sf::Text m_nameLabel;
    sf::Text m_loadGameTitle;

    Screen      m_currentScreen;
    ActiveField m_activeField;

    Button m_newGame;
    Button m_loadGame;
    Button m_statistics;
    Button m_instructions;
    Button m_exitButton;
    Button m_backButton;
    Button m_createPlayerButton;

    InputField m_nameInput;

    std::vector<Button> m_slotButtons;  // 10 shared save-slot buttons

    bool m_statsSlotSelected = false;   // true when a stats slot has been loaded
};
