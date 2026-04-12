// Description: Implementation file for the CasinoGameUI game selection/controller class.
// Related Files: CasinoGameUI.h, Player.h, Blackjack.h, HorseRace.h, Roulette.h, Poker.h, Audio.h
// Date Created: 4/7/2026
// Last Edited: 4/11/2026

#include "UIheaders/CasinoGameUI.h"

#include "Blackjack.h"
#include "UIheaders/BlackjackUI.h"
#include "HorseRace.h"
#include "UIHeaders/HorseRaceUI.h"
#include "Roulette.h"
#include "Poker.h"
#include "Audio.h"

#include <optional>
#include <sstream>
#include <memory>

CasinoGameUI::Button::Button(const sf::Font& font,
    const std::string& label,
    sf::Vector2f position,
    sf::Vector2f size)
    : box(size), text(font, label, 34)
{
    box.setPosition(position);
    box.setFillColor(sf::Color(40, 40, 60, 220));
    box.setOutlineColor(sf::Color::White);
    box.setOutlineThickness(2.f);

    text.setFillColor(sf::Color::White);

    const sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.f,
        textBounds.position.y + textBounds.size.y / 2.f
        });

    const sf::FloatRect boxBounds = box.getGlobalBounds();
    text.setPosition({
        boxBounds.position.x + boxBounds.size.x / 2.f,
        boxBounds.position.y + boxBounds.size.y / 2.f
        });
}

bool CasinoGameUI::Button::contains(sf::Vector2f point) const
{
    return box.getGlobalBounds().contains(point);
}

void CasinoGameUI::Button::update(const sf::RenderWindow& window)
{
    const sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    const sf::Vector2f mouseCoords =
        window.mapPixelToCoords(mousePixel, window.getDefaultView());

    if (contains(mouseCoords))
        box.setFillColor(sf::Color(90, 90, 130, 240));
    else
        box.setFillColor(sf::Color(40, 40, 60, 220));
}

bool CasinoGameUI::Button::isClicked(const sf::Event& event, const sf::RenderWindow& window) const
{
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePressed->button == sf::Mouse::Button::Left)
        {
            const sf::Vector2f mouseCoords =
                window.mapPixelToCoords(mousePressed->position, window.getDefaultView());

            return contains(mouseCoords);
        }
    }

    return false;
}

void CasinoGameUI::Button::draw(sf::RenderWindow& window) const
{
    window.draw(box);
    window.draw(text);
}

CasinoGameUI::InputField::InputField(const sf::Font& font,
    const std::string& placeholderText,
    sf::Vector2f position,
    sf::Vector2f size)
    : box(size),
    text(font, "", 28),
    placeholder(placeholderText)
{
    box.setPosition(position);
    box.setFillColor(sf::Color(25, 25, 35, 220));
    box.setOutlineColor(sf::Color(200, 200, 200));
    box.setOutlineThickness(2.f);

    text.setFillColor(sf::Color(210, 210, 210));
    text.setPosition({ position.x + 16.f, position.y + 12.f });

    syncText();
}

bool CasinoGameUI::InputField::contains(sf::Vector2f point) const
{
    return box.getGlobalBounds().contains(point);
}

void CasinoGameUI::InputField::setActive(bool active)
{
    if (active)
        box.setOutlineColor(sf::Color(255, 215, 0));
    else
        box.setOutlineColor(sf::Color(200, 200, 200));
}

void CasinoGameUI::InputField::syncText()
{
    if (value.empty())
    {
        text.setString(placeholder);
        text.setFillColor(sf::Color(170, 170, 170));
    }
    else
    {
        text.setString(value);
        text.setFillColor(sf::Color::White);
    }
}

void CasinoGameUI::InputField::draw(sf::RenderWindow& window) const
{
    window.draw(box);
    window.draw(text);
}

CasinoGameUI::ImageCard::ImageCard(const std::string& texturePath, sf::Vector2f position, sf::Vector2f size)
    : texture(texturePath),
    sprite(texture),
    hoverOutline(size)
{
    const sf::Vector2u textureSize = texture.getSize();

    sprite.setScale({
        size.x / static_cast<float>(textureSize.x),
        size.y / static_cast<float>(textureSize.y)
        });
    sprite.setPosition(position);

    hoverOutline.setPosition(position);
    hoverOutline.setFillColor(sf::Color::Transparent);
    hoverOutline.setOutlineThickness(4.f);
    hoverOutline.setOutlineColor(sf::Color::Transparent);
}

bool CasinoGameUI::ImageCard::contains(sf::Vector2f point) const
{
    return sprite.getGlobalBounds().contains(point);
}

void CasinoGameUI::ImageCard::update(const sf::RenderWindow& window)
{
    const sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    const sf::Vector2f mouseCoords =
        window.mapPixelToCoords(mousePixel, window.getDefaultView());

    if (contains(mouseCoords))
        hoverOutline.setOutlineColor(sf::Color(255, 215, 0));
    else
        hoverOutline.setOutlineColor(sf::Color::Transparent);
}

bool CasinoGameUI::ImageCard::isClicked(const sf::Event& event, const sf::RenderWindow& window) const
{
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePressed->button == sf::Mouse::Button::Left)
        {
            const sf::Vector2f mouseCoords =
                window.mapPixelToCoords(mousePressed->position, window.getDefaultView());

            return contains(mouseCoords);
        }
    }

    return false;
}

void CasinoGameUI::ImageCard::draw(sf::RenderWindow& window) const
{
    window.draw(sprite);
    window.draw(hoverOutline);
}

CasinoGameUI::CasinoGameUI(Player& player, AudioManager& audio)
    : m_player(player),
    m_audio(audio),
    m_running(true),
    m_saveModeActive(false),
    m_saveFieldActive(false),
    m_font("assets/arial.ttf"),
    m_backgroundTexture("assets/game_selection_background.jpg"),
    m_backgroundSprite(m_backgroundTexture),
    m_namePanelTexture("assets/player_name_panel.png"),
    m_namePanelSprite(m_namePanelTexture),
    m_balancePanelTexture("assets/balance_panel.png"),
    m_balancePanelSprite(m_balancePanelTexture),
    m_title(m_font, "GAME SELECTION", 58),
    m_status(m_font, "Choose a game or option.", 30),
    m_playerNameText(m_font, "", 28),
    m_balanceText(m_font, "", 28),
    m_horseRaceCard("assets/horse_race_card.png", { 80.f, 270.f }, { 500.f, 220.f }),
    m_blackjackCard("assets/blackjack_card.png", { 1330.f, 270.f }, { 500.f, 220.f }),
    m_rouletteCard("assets/roulette_card.png", { 390.f, 540.f }, { 500.f, 220.f }),
    m_pokerCard("assets/poker_card.png", { 1030.f, 540.f }, { 500.f, 220.f }),
    m_slotsCard("assets/slots_card.png", { 710.f, 270.f }, { 500.f, 220.f }),
    m_saveGameButton(m_font, "Save Game", { 540.f, 860.f }, { 320.f, 80.f }),
    m_returnButton(m_font, "Return to Main Menu", { 1230.f, 860.f }, { 420.f, 80.f }),
    m_confirmSaveButton(m_font, "Confirm Save", { 1250.f, 960.f }, { 280.f, 70.f }),
    m_saveFilenameInput(m_font, "Enter save filename", { 390.f, 960.f }, { 820.f, 70.f })
{
    const sf::Vector2u backgroundSize = m_backgroundTexture.getSize();
    m_backgroundSprite.setScale({
        1920.f / static_cast<float>(backgroundSize.x),
        1080.f / static_cast<float>(backgroundSize.y)
        });

    {
        const sf::Vector2u size = m_namePanelTexture.getSize();
        m_namePanelSprite.setScale({
            540.f / static_cast<float>(size.x),
            210.f / static_cast<float>(size.y)
            });
        m_namePanelSprite.setPosition({ 310.f, 60.f });
    }

    {
        const sf::Vector2u size = m_balancePanelTexture.getSize();
        m_balancePanelSprite.setScale({
            540.f / static_cast<float>(size.x),
            210.f / static_cast<float>(size.y)
            });
        m_balancePanelSprite.setPosition({ 1070.f, 60.f });
    }

    m_title.setFillColor(sf::Color::White);
    const sf::FloatRect titleBounds = m_title.getLocalBounds();
    m_title.setOrigin({
        titleBounds.position.x + titleBounds.size.x / 2.f,
        titleBounds.position.y
        });
    m_title.setPosition({ 960.f, 30.f });

    m_status.setFillColor(sf::Color::White);
    m_status.setPosition({ 60.f, 20.f });

    m_playerNameText.setFillColor(sf::Color::White);
    m_playerNameText.setPosition({ 480.f, 150.f });

    m_balanceText.setFillColor(sf::Color::White);
    m_balanceText.setPosition({ 1250.f, 150.f });

    refreshPlayerStats();
}

void CasinoGameUI::refreshPlayerStats()
{
    {
        std::ostringstream stream;
        stream << m_player.getName();
        m_playerNameText.setString(stream.str());
    }

    {
        std::ostringstream stream;
        stream << "$" << m_player.getBalance();
        m_balanceText.setString(stream.str());
    }
}

void CasinoGameUI::clearSaveInput()
{
    m_saveModeActive = false;
    m_saveFieldActive = false;
    m_saveFilenameInput.value.clear();
    m_saveFilenameInput.syncText();
    m_saveFilenameInput.setActive(false);
}

void CasinoGameUI::handleBackspace()
{
    if (m_saveFieldActive && !m_saveFilenameInput.value.empty())
    {
        m_saveFilenameInput.value.pop_back();
        m_saveFilenameInput.syncText();
    }
}

void CasinoGameUI::handleTextEntered(const sf::Event& event)
{
    if (!m_saveFieldActive)
        return;

    const auto* textEntered = event.getIf<sf::Event::TextEntered>();
    if (!textEntered)
        return;

    const char32_t unicode = textEntered->unicode;

    if (unicode == 8)
    {
        handleBackspace();
        return;
    }

    if (unicode < 32 || unicode > 126)
        return;

    if (m_saveFilenameInput.value.size() < 40)
    {
        m_saveFilenameInput.value.push_back(static_cast<char>(unicode));
        m_saveFilenameInput.syncText();
    }
}

void CasinoGameUI::saveGame()
{
    if (m_saveFilenameInput.value.empty())
    {
        m_status.setString("Save filename cannot be empty.");
        return;
    }

    if (m_player.saveToFile(m_saveFilenameInput.value))
    {
        m_status.setString("Game saved successfully.");
        clearSaveInput();
    }
    else
    {
        m_status.setString("Failed to save game.");
    }
}

enum class Screen
{
    Menu,
    HorseRace
};

Screen m_currentScreen = Screen::Menu;
std::unique_ptr<HorseRaceUI> m_horseRaceUI;

void CasinoGameUI::handleMouseClick(const sf::Event& event, sf::RenderWindow& window)
{
    if (m_horseRaceCard.isClicked(event, window))
    {
        m_audio.playSound("button");

        if (!m_horseRaceUI)
            m_horseRaceUI = std::make_unique<HorseRaceUI>(window, m_font, m_audio);

        m_horseRaceUI->open(m_player);
        m_currentScreen = Screen::HorseRace;
        m_status.setString("Horse Race loaded.");
        return;
    }

    if (m_blackjackCard.isClicked(event, window))
    {
        m_audio.playSound("button");
        BlackjackUI blackjackUI(m_player, m_audio);
        blackjackUI.run(window);
        refreshPlayerStats();
        m_status.setString("Returned from Blackjack.");
        return;
    }

    if (m_rouletteCard.isClicked(event, window))
    {
        m_audio.playSound("button");
        Roulette roulette;
        roulette.play(m_player);
        refreshPlayerStats();
        m_status.setString("Finished Roulette.");
        return;
    }

    if (m_pokerCard.isClicked(event, window))
    {
        m_audio.playSound("button");
        Poker poker;
        poker.play(m_player);
        refreshPlayerStats();
        m_status.setString("Finished Poker.");
        return;
    }

    if (m_saveGameButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        m_saveModeActive = true;
        m_status.setString("Enter a save filename.");
        return;
    }

    if (m_slotsCard.isClicked(event, window))
    {
        m_audio.playSound("button");
        m_status.setString("Slots is work in progress.");
        return;
    }

    if (m_returnButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        m_running = false;
        return;
    }

    if (!m_saveModeActive)
        return;

    const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>();
    if (mousePressed && mousePressed->button == sf::Mouse::Button::Left)
    {
        const sf::Vector2f mouseCoords =
            window.mapPixelToCoords(mousePressed->position, window.getDefaultView());

        const bool clickedSaveField = m_saveFilenameInput.contains(mouseCoords);
        m_saveFieldActive = clickedSaveField;
        m_saveFilenameInput.setActive(clickedSaveField);
    }

    if (m_confirmSaveButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        saveGame();
    }
}

void CasinoGameUI::draw(sf::RenderWindow& window)
{
    window.draw(m_backgroundSprite);
    window.draw(m_title);

    window.draw(m_namePanelSprite);
    window.draw(m_balancePanelSprite);
    window.draw(m_playerNameText);
    window.draw(m_balanceText);

    m_horseRaceCard.draw(window);
    m_blackjackCard.draw(window);
    m_rouletteCard.draw(window);
    m_pokerCard.draw(window);
    m_slotsCard.draw(window);

    m_saveGameButton.draw(window);
    m_returnButton.draw(window);

    if (m_saveModeActive)
    {
        m_saveFilenameInput.draw(window);
        m_confirmSaveButton.draw(window);
    }

    window.draw(m_status);
}

void CasinoGameUI::run(sf::RenderWindow& window)
{
    if (!m_horseRaceUI)
        m_horseRaceUI = std::make_unique<HorseRaceUI>(window, m_font, m_audio);

    sf::Clock frameClock;

    while (window.isOpen() && m_running)
    {
        m_audio.update();
        const float deltaTime = frameClock.restart().asSeconds();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
                continue;
            }

            if (m_currentScreen == Screen::HorseRace)
            {
                m_horseRaceUI->handleEvent(*event);
            }
            else
            {
                if (event->is<sf::Event::TextEntered>())
                    handleTextEntered(*event);

                handleMouseClick(*event, window);
            }
        }

        if (m_currentScreen == Screen::HorseRace)
        {
            m_horseRaceUI->update(deltaTime);

            if (m_horseRaceUI->wantsToLeave())
            {
                m_horseRaceUI->clearLeaveRequest();
                m_currentScreen = Screen::Menu;
                m_audio.playMusic("assets/sound/background/main_track.wav", true, 15.f);
                refreshPlayerStats();
                m_status.setString("Returned from Horse Race.");
            }
        }
        else
        {
            m_horseRaceCard.update(window);
            m_blackjackCard.update(window);
            m_rouletteCard.update(window);
            m_pokerCard.update(window);
            m_slotsCard.update(window);
            m_saveGameButton.update(window);
            m_returnButton.update(window);

            if (m_saveModeActive)
                m_confirmSaveButton.update(window);
        }

        window.clear();

        if (m_currentScreen == Screen::HorseRace)
            m_horseRaceUI->draw();
        else
            draw(window);

        window.display();
    }
}