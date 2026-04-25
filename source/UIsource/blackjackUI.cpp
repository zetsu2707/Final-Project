// Description: Implementation file for the Blackjack UI/controller class.
// Related Files: BlackjackUI.h, Blackjack.h, CasinoGameUI.cpp, Audio.h
// Date Created: 4/12/2026
// Last Edited: 4/12/2026

#include "UIheaders/BlackjackUI.h"
#include "Audio.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iomanip>
#include <optional>
#include <sstream>
#include <vector>

namespace
{
    std::string resolveAssetPath(const std::string& basePath)
    {
        if (std::filesystem::exists(basePath))
            return basePath;

        const std::vector<std::string> extensions = {
            ".png", ".jpg", ".jpeg", ".webp"
        };

        for (const std::string& extension : extensions)
        {
            const std::string candidate = basePath + extension;
            if (std::filesystem::exists(candidate))
                return candidate;
        }

        return basePath + ".png";
    }
}

BlackjackUI::Button::Button(const sf::Font& font,
    const std::string& label,
    sf::Vector2f position,
    sf::Vector2f size)
    : box(size), text(font, label, 32), enabled(true)
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

bool BlackjackUI::Button::contains(sf::Vector2f point) const
{
    return box.getGlobalBounds().contains(point);
}

void BlackjackUI::Button::update(const sf::RenderWindow& window)
{
    if (!enabled)
    {
        box.setFillColor(sf::Color(65, 65, 75, 180));
        text.setFillColor(sf::Color(180, 180, 180));
        return;
    }

    text.setFillColor(sf::Color::White);

    const sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    const sf::Vector2f mouseCoords =
        window.mapPixelToCoords(mousePixel, window.getDefaultView());

    if (contains(mouseCoords))
        box.setFillColor(sf::Color(90, 90, 130, 240));
    else
        box.setFillColor(sf::Color(40, 40, 60, 220));
}

bool BlackjackUI::Button::isClicked(const sf::Event& event, const sf::RenderWindow& window) const
{
    if (!enabled)
        return false;

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

void BlackjackUI::Button::draw(sf::RenderWindow& window) const
{
    window.draw(box);
    window.draw(text);
}

BlackjackUI::InputField::InputField(const sf::Font& font,
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

bool BlackjackUI::InputField::contains(sf::Vector2f point) const
{
    return box.getGlobalBounds().contains(point);
}

void BlackjackUI::InputField::setActive(bool active)
{
    if (active)
        box.setOutlineColor(sf::Color(255, 215, 0));
    else
        box.setOutlineColor(sf::Color(200, 200, 200));
}

void BlackjackUI::InputField::syncText()
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

void BlackjackUI::InputField::draw(sf::RenderWindow& window) const
{
    window.draw(box);
    window.draw(text);
}

BlackjackUI::BlackjackUI(Player& player, AudioManager& audio)
    : m_player(player),
    m_audio(audio),
    m_blackjack(),
    m_running(true),
    m_betFieldActive(false),
    m_font("assets/arial.ttf"),
    m_backgroundTexture(resolveAssetPath("assets/blackjack/blackjack_background")),
    m_backgroundSprite(m_backgroundTexture),
    m_tableTexture("assets/blackjack/table.png"),
    m_tableSprite(m_tableTexture),
    m_title(m_font, "BLACKJACK", 58),
    m_status(m_font, "", 28),
    m_balanceText(m_font, "", 28),
    m_currentBetText(m_font, "", 28),
    m_dealerValueText(m_font, "", 30),
    m_playerValueText(m_font, "", 30),
    m_betInput(m_font, "Enter bet", { 120.f, 930.f }, { 280.f, 70.f }),
    m_dealButton(m_font, "Deal", { 470.f, 930.f }, { 200.f, 70.f }),
    m_hitButton(m_font, "Hit", { 730.f, 930.f }, { 200.f, 70.f }),
    m_standButton(m_font, "Stand", { 990.f, 930.f }, { 200.f, 70.f }),
    m_backButton(m_font, "Back", { 1520.f, 930.f }, { 220.f, 70.f }),
    m_cardSize({ 140.f, 200.f })
{
    const sf::Vector2u backgroundSize = m_backgroundTexture.getSize();
    if (backgroundSize.x > 0 && backgroundSize.y > 0)
    {
        m_backgroundSprite.setScale({
            1920.f / static_cast<float>(backgroundSize.x),
            1080.f / static_cast<float>(backgroundSize.y)
            });
    }

    const sf::Vector2u tableSize = m_tableTexture.getSize();
    if (tableSize.x > 0 && tableSize.y > 0)
    {
        const float maxWidth = 1620.f;
        const float maxHeight = 760.f;
        const float scale = std::min(
            maxWidth / static_cast<float>(tableSize.x),
            maxHeight / static_cast<float>(tableSize.y));

        m_tableSprite.setScale({ scale, scale });
    }

    const sf::FloatRect tableBounds = m_tableSprite.getGlobalBounds();
    m_tableSprite.setPosition({
        960.f - tableBounds.size.x / 2.f,
        480.f - tableBounds.size.y / 2.f
        });

    const sf::FloatRect placedTableBounds = m_tableSprite.getGlobalBounds();
    m_cardSize = {
        std::clamp(placedTableBounds.size.x * 0.095f, 90.f, 145.f),
        std::clamp(placedTableBounds.size.x * 0.138f, 130.f, 210.f)
    };

    m_title.setFillColor(sf::Color::White);
    {
        const sf::FloatRect bounds = m_title.getLocalBounds();
        m_title.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });
        m_title.setPosition({ 960.f, 20.f });
    }

    m_status.setFillColor(sf::Color::White);
    m_status.setPosition({ 960.f, 850.f });

    m_balanceText.setFillColor(sf::Color::White);
    m_balanceText.setPosition({ 80.f, 30.f });

    m_currentBetText.setFillColor(sf::Color::White);
    m_currentBetText.setPosition({ 80.f, 70.f });

    m_dealerValueText.setFillColor(sf::Color::White);
    m_playerValueText.setFillColor(sf::Color::White);

    m_betInput.value = "10";
    m_betInput.syncText();

    m_blackjack.resetRound();
    refreshTexts();
    updateButtonStates();
}

void BlackjackUI::refreshTexts()
{
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2)
            << "Balance: $" << m_player.getBalance();
        m_balanceText.setString(stream.str());
    }

    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2)
            << "Current Bet: $" << m_blackjack.getCurrentBet();
        m_currentBetText.setString(stream.str());
    }

    m_status.setString(m_blackjack.getStatusText());
    {
        const sf::FloatRect bounds = m_status.getLocalBounds();
        m_status.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });
    }

    {
        std::ostringstream stream;
        if (m_blackjack.isDealerHoleCardHidden())
            stream << "Dealer Value: " << m_blackjack.getDealerVisibleValue() << " + ?";
        else
            stream << "Dealer Value: " << m_blackjack.getDealerValue();

        m_dealerValueText.setString(stream.str());
    }

    {
        std::ostringstream stream;
        stream << "Player Value: " << m_blackjack.getPlayerValue();
        m_playerValueText.setString(stream.str());
    }

    const sf::FloatRect tableBounds = m_tableSprite.getGlobalBounds();
    const float centerX = tableBounds.position.x + tableBounds.size.x / 2.f;

    const float dealerCardsCenterY = tableBounds.position.y + tableBounds.size.y * 0.24f;
    const float playerCardsCenterY = tableBounds.position.y + tableBounds.size.y * 0.67f;

    {
        const sf::FloatRect bounds = m_dealerValueText.getLocalBounds();
        m_dealerValueText.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });
        m_dealerValueText.setPosition({
            centerX,
            dealerCardsCenterY - (m_cardSize.y / 2.f) - 32.f
            });
    }

    {
        const sf::FloatRect bounds = m_playerValueText.getLocalBounds();
        m_playerValueText.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });
        m_playerValueText.setPosition({
            centerX,
            playerCardsCenterY + (m_cardSize.y / 2.f) + 18.f
            });
    }
}

void BlackjackUI::updateButtonStates()
{
    const bool roundActive =
        m_blackjack.getRoundState() == Blackjack::RoundState::PlayerTurn ||
        m_blackjack.getRoundState() == Blackjack::RoundState::DealerTurn;

    const bool playerTurn =
        m_blackjack.getRoundState() == Blackjack::RoundState::PlayerTurn;

    m_dealButton.enabled = !roundActive;
    m_hitButton.enabled = playerTurn;
    m_standButton.enabled = playerTurn;
    m_backButton.enabled = !roundActive;
}

bool BlackjackUI::isPlayerWinningResult(Blackjack::RoundResult result) const
{
    return result == Blackjack::RoundResult::PlayerBlackjack ||
        result == Blackjack::RoundResult::DealerBust ||
        result == Blackjack::RoundResult::PlayerWin;
}

void BlackjackUI::playRoundResultSound()
{
    if (!m_blackjack.isRoundOver())
        return;

    if (!m_roundStatsRecorded) {
        m_player.recordGameResult("Blackjack", isPlayerWinningResult(m_blackjack.getRoundResult()));
        m_roundStatsRecorded = true;
    }

    if (isPlayerWinningResult(m_blackjack.getRoundResult()))
        m_audio.playSound("win");
}

void BlackjackUI::handleBackspace()
{
    if (m_betFieldActive && !m_betInput.value.empty())
    {
        m_betInput.value.pop_back();
        m_betInput.syncText();
    }
}

void BlackjackUI::handleTextEntered(const sf::Event& event)
{
    if (!m_betFieldActive)
        return;

    if (m_blackjack.getRoundState() == Blackjack::RoundState::PlayerTurn ||
        m_blackjack.getRoundState() == Blackjack::RoundState::DealerTurn)
    {
        return;
    }

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

    const char character = static_cast<char>(unicode);

    if ((character >= '0' && character <= '9') || character == '.')
    {
        if (character == '.' && m_betInput.value.find('.') != std::string::npos)
            return;

        if (m_betInput.value.size() < 10)
        {
            m_betInput.value.push_back(character);
            m_betInput.syncText();
        }
    }
}

void BlackjackUI::handleMouseClick(const sf::Event& event, sf::RenderWindow& window)
{
    const bool roundActive =
        m_blackjack.getRoundState() == Blackjack::RoundState::PlayerTurn ||
        m_blackjack.getRoundState() == Blackjack::RoundState::DealerTurn;

    if (!roundActive && m_backButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        m_running = false;
        return;
    }

    if (m_dealButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        m_betFieldActive = false;
        m_betInput.setActive(false);

        m_roundStatsRecorded = false;
        const bool roundStarted = m_blackjack.startRound(m_player, getParsedBet());

        if (roundStarted)
            m_audio.playSound("card");

        playRoundResultSound();
        refreshTexts();
        updateButtonStates();
        return;
    }

    if (m_hitButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        m_audio.playSound("card");

        m_blackjack.playerHit(m_player);

        playRoundResultSound();
        refreshTexts();
        updateButtonStates();
        return;
    }

    if (m_standButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        m_blackjack.playerStand(m_player);

        playRoundResultSound();
        refreshTexts();
        updateButtonStates();
        return;
    }

    const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>();
    if (mousePressed && mousePressed->button == sf::Mouse::Button::Left)
    {
        const sf::Vector2f mouseCoords =
            window.mapPixelToCoords(mousePressed->position, window.getDefaultView());

        const bool clickedBetField = m_betInput.contains(mouseCoords);
        const bool canEditBet =
            m_blackjack.getRoundState() != Blackjack::RoundState::PlayerTurn &&
            m_blackjack.getRoundState() != Blackjack::RoundState::DealerTurn;

        m_betFieldActive = clickedBetField && canEditBet;
        m_betInput.setActive(m_betFieldActive);
    }
}

void BlackjackUI::draw(sf::RenderWindow& window)
{
    window.draw(m_backgroundSprite);
    drawTableScene(window);

    window.draw(m_title);
    window.draw(m_balanceText);
    window.draw(m_currentBetText);
    window.draw(m_status);
    window.draw(m_dealerValueText);
    window.draw(m_playerValueText);

    m_betInput.draw(window);
    m_dealButton.draw(window);
    m_hitButton.draw(window);
    m_standButton.draw(window);
    m_backButton.draw(window);
}

void BlackjackUI::drawTableScene(sf::RenderWindow& window)
{
    window.draw(m_tableSprite);

    const sf::FloatRect tableBounds = m_tableSprite.getGlobalBounds();

    const sf::Vector2f deckBasePosition = {
        tableBounds.position.x + tableBounds.size.x * 0.11f,
        tableBounds.position.y + tableBounds.size.y * 0.20f
    };

    Blackjack::CardView hiddenCard;
    for (int i = 0; i < 3; ++i)
    {
        drawCard(window,
            hiddenCard,
            { deckBasePosition.x + i * 4.f, deckBasePosition.y - i * 4.f },
            true);
    }

    drawHand(window,
        m_blackjack.getDealerCards(),
        tableBounds.position.y + tableBounds.size.y * 0.24f,
        m_blackjack.isDealerHoleCardHidden());

    drawHand(window,
        m_blackjack.getPlayerCards(),
        tableBounds.position.y + tableBounds.size.y * 0.67f,
        false);
}

void BlackjackUI::drawHand(sf::RenderWindow& window,
    const std::vector<Blackjack::CardView>& cards,
    float centerY,
    bool hideFirstCard) const
{
    if (cards.empty())
        return;

    const sf::FloatRect tableBounds = m_tableSprite.getGlobalBounds();
    const float centerX = tableBounds.position.x + tableBounds.size.x / 2.f;
    const float spacing = m_cardSize.x * 0.38f;
    const float totalWidth = m_cardSize.x + static_cast<float>(cards.size() - 1) * spacing;
    const float startX = centerX - totalWidth / 2.f;
    const float topY = centerY - m_cardSize.y / 2.f;

    for (std::size_t i = 0; i < cards.size(); ++i)
    {
        drawCard(window,
            cards[i],
            { startX + static_cast<float>(i) * spacing, topY },
            hideFirstCard && i == 0);
    }
}

void BlackjackUI::drawCard(sf::RenderWindow& window,
    const Blackjack::CardView& card,
    sf::Vector2f position,
    bool hidden) const
{
    if (hidden)
    {
        sf::RectangleShape back(m_cardSize);
        back.setPosition(position);
        back.setFillColor(sf::Color(18, 18, 28));
        back.setOutlineColor(sf::Color(255, 215, 0));
        back.setOutlineThickness(3.f);
        window.draw(back);

        sf::RectangleShape inner({
            m_cardSize.x - 18.f,
            m_cardSize.y - 18.f
            });
        inner.setPosition({ position.x + 9.f, position.y + 9.f });
        inner.setFillColor(sf::Color(45, 45, 70));
        inner.setOutlineColor(sf::Color(220, 220, 220));
        inner.setOutlineThickness(1.5f);
        window.draw(inner);

        sf::Text label(m_font, "BJ", 34);
        label.setFillColor(sf::Color::White);

        const sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y + bounds.size.y / 2.f
            });
        label.setPosition({
            position.x + m_cardSize.x / 2.f,
            position.y + m_cardSize.y / 2.f
            });

        window.draw(label);
        return;
    }

    const std::string texturePath = getCardTexturePath(card);
    const sf::Texture* texture = getCardTexture(texturePath);

    if (texture != nullptr)
    {
        sf::Sprite sprite(*texture);

        const sf::Vector2u textureSize = texture->getSize();
        sprite.setScale({
            m_cardSize.x / static_cast<float>(textureSize.x),
            m_cardSize.y / static_cast<float>(textureSize.y)
            });
        sprite.setPosition(position);
        window.draw(sprite);
        return;
    }

    sf::RectangleShape placeholder(m_cardSize);
    placeholder.setPosition(position);
    placeholder.setFillColor(sf::Color::White);
    placeholder.setOutlineColor(sf::Color::Black);
    placeholder.setOutlineThickness(2.f);
    window.draw(placeholder);

    const std::string suitLetter =
        card.suit.empty() ? "?" : std::string(1, static_cast<char>(std::toupper(card.suit.front())));

    sf::Text label(m_font, card.rank + "\n" + suitLetter, 24);
    label.setFillColor(sf::Color::Black);
    label.setPosition({ position.x + 12.f, position.y + 10.f });
    window.draw(label);
}

const sf::Texture* BlackjackUI::getCardTexture(const std::string& path) const
{
    if (path.empty() || !std::filesystem::exists(path))
        return nullptr;

    const auto found = m_cardTextures.find(path);
    if (found != m_cardTextures.end())
        return found->second.get();

    auto texture = std::make_unique<sf::Texture>(path);
    const sf::Texture* texturePtr = texture.get();
    m_cardTextures.emplace(path, std::move(texture));
    return texturePtr;
}

std::string BlackjackUI::getCardTexturePath(const Blackjack::CardView& card) const
{
    return "assets/cards/" + toLower(card.rank) + "_of_" + toLower(card.suit) + ".png";
}

double BlackjackUI::getParsedBet() const
{
    try
    {
        if (m_betInput.value.empty())
            return 0.0;

        return std::stod(m_betInput.value);
    }
    catch (...)
    {
        return 0.0;
    }
}

std::string BlackjackUI::toLower(const std::string& text)
{
    std::string value = text;
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char ch)
        {
            return static_cast<char>(std::tolower(ch));
        });
    return value;
}

void BlackjackUI::run(sf::RenderWindow& window)
{
    m_audio.playMusicFadeIn("assets/sound/background/blackjack_track.wav", 1.5f, true, 15.f);

    while (window.isOpen() && m_running)
    {
        m_audio.update();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
                continue;
            }

            if (event->is<sf::Event::TextEntered>())
                handleTextEntered(*event);

            handleMouseClick(*event, window);
        }

        refreshTexts();
        updateButtonStates();

        m_dealButton.update(window);
        m_hitButton.update(window);
        m_standButton.update(window);
        m_backButton.update(window);

        window.clear();
        draw(window);
        window.display();
    }

    if (window.isOpen())
        m_audio.playMusic("assets/sound/background/main_track.wav", true, 15.f);
}