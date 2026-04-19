// Description: Implementation file for the Poker UI/controller class.
// Related Files: PokerUI.h, Poker.h, CasinoGameUI.cpp, Audio.h
// Date Created: 4/13/2026
// Last Edited: 4/19/2026

#include "UIheaders/PokerUI.h"
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

    std::string formatMoney(double amount)
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << amount;
        return stream.str();
    }
}

PokerUI::Button::Button(const sf::Font& font,
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
    setLabel(label);
}

bool PokerUI::Button::contains(sf::Vector2f point) const
{
    return box.getGlobalBounds().contains(point);
}

void PokerUI::Button::setLabel(const std::string& label)
{
    text.setString(label);

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

void PokerUI::Button::update(const sf::RenderWindow& window)
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

bool PokerUI::Button::isClicked(const sf::Event& event, const sf::RenderWindow& window) const
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

void PokerUI::Button::draw(sf::RenderWindow& window) const
{
    window.draw(box);
    window.draw(text);
}

PokerUI::InputField::InputField(const sf::Font& font,
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

bool PokerUI::InputField::contains(sf::Vector2f point) const
{
    return box.getGlobalBounds().contains(point);
}

void PokerUI::InputField::setActive(bool active)
{
    if (active)
        box.setOutlineColor(sf::Color(255, 215, 0));
    else
        box.setOutlineColor(sf::Color(200, 200, 200));
}

void PokerUI::InputField::syncText()
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

void PokerUI::InputField::draw(sf::RenderWindow& window) const
{
    window.draw(box);
    window.draw(text);
}

PokerUI::PokerUI(Player& player, AudioManager& audio)
    : m_player(player),
    m_audio(audio),
    m_poker(),
    m_running(true),
    m_wagerFieldActive(false),
    m_font("assets/arial.ttf"),
    m_backgroundTexture(resolveAssetPath("assets/poker/poker_background")),
    m_backgroundSprite(m_backgroundTexture),
    m_tableTexture("assets/poker/table.png"),
    m_tableSprite(m_tableTexture),
    m_title(m_font, "TEXAS HOLD'EM", 58),
    m_status(m_font, "", 28),
    m_balanceText(m_font, "", 28),
    m_potText(m_font, "", 28),
    m_phaseText(m_font, "", 28),
    m_currentBetText(m_font, "", 28),
    m_playerContributionText(m_font, "", 28),
    m_showdownText(m_font, "", 28),
    m_communityLabel(m_font, "Board", 34),
    m_playerLabel(m_font, "You", 38),
    m_wagerInput(m_font, "Enter ante", { 120.f, 930.f }, { 280.f, 70.f }),
    m_dealButton(m_font, "Deal", { 470.f, 930.f }, { 200.f, 70.f }),
    m_foldButton(m_font, "Fold", { 730.f, 930.f }, { 200.f, 70.f }),
    m_checkCallButton(m_font, "Check", { 990.f, 930.f }, { 220.f, 70.f }),
    m_raiseButton(m_font, "Raise", { 1270.f, 930.f }, { 200.f, 70.f }),
    m_backButton(m_font, "Back", { 1520.f, 930.f }, { 220.f, 70.f }),
    m_opponentSeats({
        SeatAnchor{0.23f, 0.30f, true},
        SeatAnchor{0.50f, 0.18f, true},
        SeatAnchor{0.77f, 0.30f, true}
        }),
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
        510.f - tableBounds.size.y / 2.f
        });

    const sf::FloatRect placedTableBounds = m_tableSprite.getGlobalBounds();
    m_cardSize = {
        std::clamp(placedTableBounds.size.x * 0.090f, 85.f, 135.f),
        std::clamp(placedTableBounds.size.x * 0.132f, 120.f, 195.f)
    };

    m_title.setFillColor(sf::Color::White);
    {
        const sf::FloatRect bounds = m_title.getLocalBounds();
        m_title.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });
        m_title.setPosition({ 960.f, 18.f });
    }

    m_status.setFillColor(sf::Color::White);
    m_status.setPosition({ 960.f, 108.f });

    m_balanceText.setFillColor(sf::Color::White);
    m_balanceText.setPosition({ 80.f, 24.f });

    m_potText.setFillColor(sf::Color::White);
    m_potText.setPosition({ 80.f, 62.f });

    m_phaseText.setFillColor(sf::Color::White);
    m_phaseText.setPosition({ 80.f, 100.f });

    m_currentBetText.setFillColor(sf::Color::White);
    m_currentBetText.setPosition({ 1500.f, 24.f });

    m_playerContributionText.setFillColor(sf::Color::White);
    m_playerContributionText.setPosition({ 1500.f, 62.f });

    m_showdownText.setFillColor(sf::Color::White);
    m_showdownText.setPosition({ 960.f, 72.f });

    m_communityLabel.setFillColor(sf::Color::White);
    m_playerLabel.setFillColor(sf::Color::White);

    m_wagerInput.value = "10";
    m_wagerInput.syncText();

    m_poker.resetRound();
    refreshTexts();
    updateWagerFieldMode();
    updateButtonStates();
}

void PokerUI::refreshTexts()
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
            << "Pot: $" << m_poker.getPot();
        m_potText.setString(stream.str());
    }

    {
        std::ostringstream stream;
        stream << "Phase: " << m_poker.getPhaseText();
        m_phaseText.setString(stream.str());
    }

    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2)
            << "Current Bet: $" << m_poker.getCurrentBet();
        m_currentBetText.setString(stream.str());
    }

    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2)
            << "Your Street Bet: $" << m_poker.getPlayerContribution();
        m_playerContributionText.setString(stream.str());
    }

    m_status.setString(m_poker.getStatusText());
    {
        const sf::FloatRect bounds = m_status.getLocalBounds();
        m_status.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });
    }

    m_showdownText.setString("");
    if (!m_poker.getWinningSummaryText().empty())
    {
        m_showdownText.setString(m_poker.getWinningSummaryText());
        const sf::FloatRect bounds = m_showdownText.getLocalBounds();
        m_showdownText.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });
    }

    const sf::FloatRect tableBounds = m_tableSprite.getGlobalBounds();
    const float centerX = tableBounds.position.x + tableBounds.size.x / 2.f;
    const float communityCenterY = tableBounds.position.y + tableBounds.size.y * 0.52f;
    const float playerCenterY = tableBounds.position.y + tableBounds.size.y * 0.84f;

    {
        const sf::FloatRect bounds = m_communityLabel.getLocalBounds();
        m_communityLabel.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });
        m_communityLabel.setPosition({
            centerX,
            communityCenterY - m_cardSize.y / 2.f - 46.f
            });
    }

    {
        const sf::FloatRect bounds = m_playerLabel.getLocalBounds();
        m_playerLabel.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });
        m_playerLabel.setPosition({
            centerX,
            playerCenterY - m_cardSize.y / 2.f - 42.f
            });
    }

    const double amountToCall = m_poker.getAmountToCall();
    if (amountToCall > 0.0)
        m_checkCallButton.setLabel("Call");
    else
        m_checkCallButton.setLabel("Check");
}

void PokerUI::updateButtonStates()
{
    const bool playerTurn = m_poker.isPlayerTurn();
    const bool roundActive = playerTurn ||
        m_poker.getRoundState() == Poker::RoundState::Showdown;

    m_dealButton.enabled = !roundActive;
    m_foldButton.enabled = playerTurn;
    m_checkCallButton.enabled = playerTurn;
    m_raiseButton.enabled = playerTurn;
    m_backButton.enabled = !roundActive;
}

void PokerUI::updateWagerFieldMode()
{
    if (m_poker.isPlayerTurn())
        m_wagerInput.placeholder = "Raise amount";
    else
        m_wagerInput.placeholder = "Enter ante";

    m_wagerInput.syncText();
}

bool PokerUI::isPlayerWinningResult(Poker::RoundResult result) const
{
    return result == Poker::RoundResult::PlayerWin ||
        result == Poker::RoundResult::SplitPot;
}

void PokerUI::playRoundResultSound()
{
    if (!m_poker.isRoundOver())
        return;

    if (isPlayerWinningResult(m_poker.getRoundResult()))
        m_audio.playSound("win");
}

void PokerUI::handleBackspace()
{
    if (m_wagerFieldActive && !m_wagerInput.value.empty())
    {
        m_wagerInput.value.pop_back();
        m_wagerInput.syncText();
    }
}

void PokerUI::handleTextEntered(const sf::Event& event)
{
    if (!m_wagerFieldActive)
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

    const char character = static_cast<char>(unicode);

    if ((character >= '0' && character <= '9') || character == '.')
    {
        if (character == '.' && m_wagerInput.value.find('.') != std::string::npos)
            return;

        if (m_wagerInput.value.size() < 10)
        {
            m_wagerInput.value.push_back(character);
            m_wagerInput.syncText();
        }
    }
}

void PokerUI::handleMouseClick(const sf::Event& event, sf::RenderWindow& window)
{
    const bool roundActive = m_poker.isPlayerTurn() ||
        m_poker.getRoundState() == Poker::RoundState::Showdown;

    if (!roundActive && m_backButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        m_running = false;
        return;
    }

    if (m_dealButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        m_wagerFieldActive = false;
        m_wagerInput.setActive(false);

        const bool roundStarted = m_poker.startRound(m_player, getParsedWager());

        if (roundStarted)
            m_audio.playSound("card");

        refreshTexts();
        updateWagerFieldMode();
        updateButtonStates();
        return;
    }

    if (m_foldButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        m_poker.playerFold(m_player);

        playRoundResultSound();
        refreshTexts();
        updateWagerFieldMode();
        updateButtonStates();
        return;
    }

    if (m_checkCallButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        m_audio.playSound("card");

        m_poker.playerCheckCall(m_player);

        playRoundResultSound();
        refreshTexts();
        updateWagerFieldMode();
        updateButtonStates();
        return;
    }

    if (m_raiseButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        m_audio.playSound("card");

        m_poker.playerRaise(m_player, getParsedWager());

        playRoundResultSound();
        refreshTexts();
        updateWagerFieldMode();
        updateButtonStates();
        return;
    }

    const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>();
    if (mousePressed && mousePressed->button == sf::Mouse::Button::Left)
    {
        const sf::Vector2f mouseCoords =
            window.mapPixelToCoords(mousePressed->position, window.getDefaultView());

        const bool clickedWagerField = m_wagerInput.contains(mouseCoords);
        m_wagerFieldActive = clickedWagerField;
        m_wagerInput.setActive(m_wagerFieldActive);
    }
}

void PokerUI::draw(sf::RenderWindow& window)
{
    window.draw(m_backgroundSprite);
    drawTableScene(window);

    window.draw(m_title);
    window.draw(m_balanceText);
    window.draw(m_potText);
    window.draw(m_phaseText);
    window.draw(m_currentBetText);
    window.draw(m_playerContributionText);
    window.draw(m_status);

    if (!m_showdownText.getString().isEmpty())
        window.draw(m_showdownText);

    m_wagerInput.draw(window);
    m_dealButton.draw(window);
    m_foldButton.draw(window);
    m_checkCallButton.draw(window);
    m_raiseButton.draw(window);
    m_backButton.draw(window);
}

void PokerUI::drawTableScene(sf::RenderWindow& window)
{
    window.draw(m_tableSprite);

    const sf::FloatRect tableBounds = m_tableSprite.getGlobalBounds();

    const sf::Vector2f deckBasePosition = {
        tableBounds.position.x + tableBounds.size.x * 0.00f,
        tableBounds.position.y + tableBounds.size.y * 0.23f
    };

    Poker::CardView hiddenCard;
    for (int i = 0; i < 3; ++i)
    {
        drawCard(window,
            hiddenCard,
            { deckBasePosition.x + i * 4.f, deckBasePosition.y - i * 4.f },
            true);
    }

    drawOpponents(window);

    drawCommunityRow(window,
        m_poker.getCommunityCards(),
        tableBounds.position.x + tableBounds.size.x / 2.f,
        tableBounds.position.y + tableBounds.size.y * 0.52f);

    window.draw(m_communityLabel);

    drawHand(window,
        m_poker.getPlayerCards(),
        tableBounds.position.x + tableBounds.size.x / 2.f,
        tableBounds.position.y + tableBounds.size.y * 0.84f,
        false);

    window.draw(m_playerLabel);
}

void PokerUI::drawOpponents(sf::RenderWindow& window) const
{
    const std::vector<Poker::OpponentView> opponents = m_poker.getOpponents();
    const sf::FloatRect tableBounds = m_tableSprite.getGlobalBounds();

    for (std::size_t i = 0; i < opponents.size() && i < m_opponentSeats.size(); ++i)
    {
        const SeatAnchor& seat = m_opponentSeats[i];
        const sf::Vector2f center = {
            tableBounds.position.x + tableBounds.size.x * seat.centerXFactor,
            tableBounds.position.y + tableBounds.size.y * seat.centerYFactor
        };

        drawOpponentSeat(window, opponents[i], center, seat.labelAbove);
    }
}

void PokerUI::drawOpponentSeat(sf::RenderWindow& window,
    const Poker::OpponentView& opponent,
    sf::Vector2f center,
    bool labelAbove) const
{
    const bool hideCards = m_poker.areOpponentHoleCardsHidden() && !opponent.folded;
    drawHand(window, opponent.cards, center.x, center.y, hideCards);

    std::ostringstream labelStream;
    labelStream << opponent.name;

    if (opponent.folded)
        labelStream << " (Folded)";
    else if (!opponent.bestHandText.empty())
        labelStream << " - " << opponent.bestHandText;
    else if (opponent.contribution > 0.0)
        labelStream << " $" << std::fixed << std::setprecision(2) << opponent.contribution;

    sf::Text label(m_font, labelStream.str(), 26);
    label.setFillColor(sf::Color::White);

    const sf::FloatRect bounds = label.getLocalBounds();
    label.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y
        });

    const float labelY = labelAbove
        ? center.y - m_cardSize.y / 2.f - 38.f
        : center.y + m_cardSize.y / 2.f + 12.f;

    label.setPosition({ center.x, labelY });
    window.draw(label);
}

void PokerUI::drawHand(sf::RenderWindow& window,
    const std::vector<Poker::CardView>& cards,
    float centerX,
    float centerY,
    bool hideCards) const
{
    if (cards.empty())
        return;

    const float spacing = m_cardSize.x * 0.42f;
    const float totalWidth = m_cardSize.x + static_cast<float>(cards.size() - 1) * spacing;
    const float startX = centerX - totalWidth / 2.f;
    const float topY = centerY - m_cardSize.y / 2.f;

    for (std::size_t i = 0; i < cards.size(); ++i)
    {
        drawCard(window,
            cards[i],
            { startX + static_cast<float>(i) * spacing, topY },
            hideCards);
    }
}

void PokerUI::drawCommunityRow(sf::RenderWindow& window,
    const std::vector<Poker::CardView>& cards,
    float centerX,
    float centerY) const
{
    const float spacing = m_cardSize.x * 0.34f;
    const float totalWidth = m_cardSize.x * 5.f + spacing * 4.f;
    const float startX = centerX - totalWidth / 2.f;
    const float topY = centerY - m_cardSize.y / 2.f;

    for (int i = 0; i < 5; ++i)
    {
        const sf::Vector2f position = {
            startX + static_cast<float>(i) * (m_cardSize.x + spacing),
            topY
        };

        if (i < static_cast<int>(cards.size()))
            drawCard(window, cards[static_cast<std::size_t>(i)], position, false);
        else
            drawEmptyCardSlot(window, position);
    }
}

void PokerUI::drawCard(sf::RenderWindow& window,
    const Poker::CardView& card,
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

        sf::Text label(m_font, "POKER", 26);
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

void PokerUI::drawEmptyCardSlot(sf::RenderWindow& window, sf::Vector2f position) const
{
    sf::RectangleShape slot(m_cardSize);
    slot.setPosition(position);
    slot.setFillColor(sf::Color(255, 255, 255, 22));
    slot.setOutlineColor(sf::Color(230, 230, 230, 180));
    slot.setOutlineThickness(2.f);
    window.draw(slot);
}

const sf::Texture* PokerUI::getCardTexture(const std::string& path) const
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

std::string PokerUI::getCardTexturePath(const Poker::CardView& card) const
{
    return "assets/cards/" + toLower(card.rank) + "_of_" + toLower(card.suit) + ".png";
}

double PokerUI::getParsedWager() const
{
    try
    {
        if (m_wagerInput.value.empty())
            return 0.0;

        return std::stod(m_wagerInput.value);
    }
    catch (...)
    {
        return 0.0;
    }
}

std::string PokerUI::toLower(const std::string& text)
{
    std::string value = text;
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char ch)
        {
            return static_cast<char>(std::tolower(ch));
        });
    return value;
}

void PokerUI::run(sf::RenderWindow& window)
{
    m_audio.playMusicFadeIn("assets/sound/background/poker_track.wav", 1.5f, true, 15.f);

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
        updateWagerFieldMode();
        updateButtonStates();

        m_dealButton.update(window);
        m_foldButton.update(window);
        m_checkCallButton.update(window);
        m_raiseButton.update(window);
        m_backButton.update(window);

        window.clear();
        draw(window);
        window.display();
    }

    if (window.isOpen())
        m_audio.playMusic("assets/sound/background/main_track.wav", true, 15.f);
}
