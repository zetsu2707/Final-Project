// Description: Implementation file for the Roulette UI/controller class.
// Related Files: routtleui.h, Roulette.cpp, Player.h, Audio.h
// Date Created: 4/21/2026
// Last Edited: 4/21/2026

#include "uiheaders/rouletteUI.h"

#include "Audio.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <optional>
#include <sstream>
#include <vector>

namespace
{
    constexpr float kWindowWidth = 1920.f;
    constexpr float kWindowHeight = 1080.f;
    constexpr float kWheelVisualCalibrationDegrees = -2;

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

    float easeOutCubic(float t)
    {
        const float remaining = 1.f - t;
        return 1.f - remaining * remaining * remaining;
    }

    float normalizeAngle(float angle)
    {
        angle = std::fmod(angle, 360.f);
        if (angle < 0.f)
            angle += 360.f;
        return angle;
    }

    int getPocketIndex(int number)
    {
        static const std::array<int, 37> wheelOrder = {
            0, 32, 15, 19, 4, 21, 2, 25, 17, 34,
            6, 27, 13, 36, 11, 30, 8, 23, 10, 5,
            24, 16, 33, 1, 20, 14, 31, 9, 22, 18,
            29, 7, 28, 12, 35, 3, 26
        };

        for (std::size_t i = 0; i < wheelOrder.size(); ++i)
        {
            if (wheelOrder[i] == number)
                return static_cast<int>(i);
        }

        return 0;
    }

    float getWheelStopOffset(int number)
    {
        const float anglePerPocket = 360.f / 37.f;
        return static_cast<float>(getPocketIndex(number)) * anglePerPocket
            + anglePerPocket * 0.5f;
    }

    sf::Color getPocketDisplayColor(Roulette::PocketColor color)
    {
        switch (color)
        {
        case Roulette::PocketColor::Red:
            return sf::Color(190, 45, 45);

        case Roulette::PocketColor::Black:
            return sf::Color(35, 35, 35);

        case Roulette::PocketColor::Green:
        default:
            return sf::Color(15, 135, 65);
        }
    }
}

RouletteUI::Button::Button(const sf::Font& font,
    const std::string& label,
    sf::Vector2f position,
    sf::Vector2f size)
    : box(size),
    text(font, label, 30),
    selected(false),
    enabled(true)
{
    box.setPosition(position);
    box.setFillColor(sf::Color(40, 40, 60, 215));
    box.setOutlineColor(sf::Color(245, 210, 110));
    box.setOutlineThickness(2.f);

    text.setFillColor(sf::Color::White);
    setLabel(label);
}

bool RouletteUI::Button::contains(sf::Vector2f point) const
{
    return box.getGlobalBounds().contains(point);
}

void RouletteUI::Button::setLabel(const std::string& label)
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

void RouletteUI::Button::update(const sf::RenderWindow& window)
{
    if (!enabled)
    {
        box.setFillColor(sf::Color(70, 70, 85, 155));
        box.setOutlineColor(sf::Color(140, 140, 140));
        text.setFillColor(sf::Color(185, 185, 185));
        return;
    }

    text.setFillColor(sf::Color::White);

    const sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    const sf::Vector2f mouseCoords =
        window.mapPixelToCoords(mousePixel, window.getDefaultView());

    if (selected)
    {
        box.setFillColor(sf::Color(140, 102, 20, 235));
        box.setOutlineColor(sf::Color(255, 221, 102));
    }
    else if (contains(mouseCoords))
    {
        box.setFillColor(sf::Color(96, 72, 28, 235));
        box.setOutlineColor(sf::Color(255, 235, 150));
    }
    else
    {
        box.setFillColor(sf::Color(40, 40, 60, 215));
        box.setOutlineColor(sf::Color(245, 210, 110));
    }
}

bool RouletteUI::Button::isClicked(const sf::Event& event, const sf::RenderWindow& window) const
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

void RouletteUI::Button::draw(sf::RenderWindow& window) const
{
    window.draw(box);
    window.draw(text);
}

RouletteUI::InputField::InputField(const sf::Font& font,
    const std::string& placeholderText,
    sf::Vector2f position,
    sf::Vector2f size,
    bool numeric,
    bool allowDecimal)
    : box(size),
    text(font, "", 28),
    placeholder(placeholderText),
    numericOnly(numeric),
    decimalAllowed(allowDecimal)
{
    box.setPosition(position);
    box.setFillColor(sf::Color(25, 25, 35, 220));
    box.setOutlineColor(sf::Color(200, 200, 200));
    box.setOutlineThickness(2.f);

    text.setFillColor(sf::Color(210, 210, 210));
    text.setPosition({ position.x + 16.f, position.y + 12.f });

    syncText();
}

bool RouletteUI::InputField::contains(sf::Vector2f point) const
{
    return box.getGlobalBounds().contains(point);
}

void RouletteUI::InputField::setActive(bool active)
{
    if (active)
        box.setOutlineColor(sf::Color(255, 215, 0));
    else
        box.setOutlineColor(sf::Color(200, 200, 200));
}

void RouletteUI::InputField::syncText()
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

void RouletteUI::InputField::draw(sf::RenderWindow& window) const
{
    window.draw(box);
    window.draw(text);
}

RouletteUI::RouletteUI(Player& player, AudioManager& audio)
    : m_player(player),
    m_audio(audio),
    m_roulette(),
    m_running(true),
    m_spinning(false),
    m_activeField(ActiveField::None),
    m_selectedBetType(Roulette::BetType::Red),
    m_lastOutcome(),
    m_pendingOutcome(),
    m_hasLastOutcome(false),
    m_hasPendingOutcome(false),
    m_wheelAngle(0.f),
    m_spinStartAngle(0.f),
    m_spinTargetAngle(0.f),
    m_spinDurationSeconds(3.2f),
    m_font("assets/arial.ttf"),
    m_backgroundTexture(resolveAssetPath("assets/roulette/roulette_background")),
    m_backgroundSprite(m_backgroundTexture),
    m_tableTexture(resolveAssetPath("assets/roulette/roulette_table")),
    m_tableSprite(m_tableTexture),
    m_wheelTexture(resolveAssetPath("assets/roulette/roulette_wheel")),
    m_wheelSprite(m_wheelTexture),
    m_tableFallback({ 1120.f, 610.f }),
    m_wheelFallback(260.f),
    m_topBar({ kWindowWidth - 100.f, 108.f }),
    m_bottomPanel({ kWindowWidth - 120.f, 220.f }),
    m_sidePanel({ 620.f, 360.f }),
    m_resultPanel({ 560.f, 150.f }),
    m_resultBadge(44.f),
    m_title(m_font, "ROULETTE", 60),
    m_subtitle(m_font, "European single-zero table", 28),
    m_status(m_font, "", 28),
    m_playerNameText(m_font, "", 28),
    m_balanceText(m_font, "", 30),
    m_selectionTitle(m_font, "Select Your Bet", 34),
    m_betAmountLabel(m_font, "Bet Amount", 28),
    m_numberLabel(m_font, "Straight Number (0-36)", 28),
    m_lastSpinTitle(m_font, "Last Spin", 34),
    m_lastSpinValue(m_font, "--", 40),
    m_lastOutcomeText(m_font, "Place a wager to begin.", 24),
    m_helpText(m_font,
        "Payouts:\n"
        "� Straight number pays 35:1\n"
        "� Red / Black / Even / Odd pay 1:1\n\n"
        "Tip:\n"
        "Choose Number, then enter 0-36.\n"
        "Green 0 beats all even-money bets.",
        22),
    m_numberButton(m_font, "Number", { 110.f, 705.f }, { 220.f, 64.f }),
    m_redButton(m_font, "Red", { 360.f, 705.f }, { 180.f, 64.f }),
    m_blackButton(m_font, "Black", { 570.f, 705.f }, { 180.f, 64.f }),
    m_evenButton(m_font, "Even", { 780.f, 705.f }, { 180.f, 64.f }),
    m_oddButton(m_font, "Odd", { 990.f, 705.f }, { 180.f, 64.f }),
    m_spinButton(m_font, "Spin", { 1450.f, 888.f }, { 180.f, 70.f }),
    m_clearButton(m_font, "Clear", { 1240.f, 888.f }, { 180.f, 70.f }),
    m_returnButton(m_font, "Back", { 1660.f, 888.f }, { 160.f, 70.f }),
    m_betAmountInput(m_font, "Enter wager", { 160.f, 915.f }, { 280.f, 62.f }, true, true),
    m_numberInput(m_font, "Only for Number bet", { 520.f, 915.f }, { 320.f, 62.f }, true, false)
{
    const sf::Vector2u backgroundSize = m_backgroundTexture.getSize();
    if (backgroundSize.x > 0 && backgroundSize.y > 0)
    {
        m_backgroundSprite.setScale({
            kWindowWidth / static_cast<float>(backgroundSize.x),
            kWindowHeight / static_cast<float>(backgroundSize.y)
            });
    }

    const sf::Vector2u tableSize = m_tableTexture.getSize();
    if (tableSize.x > 0 && tableSize.y > 0)
    {
        const float scale = std::min(
            1120.f / static_cast<float>(tableSize.x),
            610.f / static_cast<float>(tableSize.y));

        m_tableSprite.setScale({ scale, scale });
        m_tableSprite.setPosition({ 60.f, 116.f });
    }

    const sf::Vector2u wheelSize = m_wheelTexture.getSize();
    if (wheelSize.x > 0 && wheelSize.y > 0)
    {
        const float targetDiameter = 520.f;

        m_wheelSprite.setOrigin({
            static_cast<float>(wheelSize.x) / 2.f,
            static_cast<float>(wheelSize.y) / 2.f
            });
        m_wheelSprite.setScale({
            targetDiameter / static_cast<float>(wheelSize.x),
            targetDiameter / static_cast<float>(wheelSize.y)
            });
        m_wheelSprite.setPosition({ 1508.f, 338.f });
    }

    m_tableFallback.setPosition({ 60.f, 180.f });
    m_tableFallback.setFillColor(sf::Color(18, 78, 46, 235));
    m_tableFallback.setOutlineColor(sf::Color(210, 180, 95));
    m_tableFallback.setOutlineThickness(4.f);

    m_wheelFallback.setOrigin({ 260.f, 260.f });
    m_wheelFallback.setPosition({ 1495.f, 338.f });
    m_wheelFallback.setFillColor(sf::Color(100, 20, 20, 225));
    m_wheelFallback.setOutlineColor(sf::Color(230, 200, 105));
    m_wheelFallback.setOutlineThickness(8.f);

    m_topBar.setPosition({ 50.f, 22.f });
    m_topBar.setFillColor(sf::Color(15, 18, 28, 200));
    m_topBar.setOutlineColor(sf::Color(255, 215, 120));
    m_topBar.setOutlineThickness(2.f);

    m_bottomPanel.setPosition({ 60.f, 790.f });
    m_bottomPanel.setFillColor(sf::Color(14, 16, 26, 220));
    m_bottomPanel.setOutlineColor(sf::Color(230, 200, 110));
    m_bottomPanel.setOutlineThickness(2.f);

    m_sidePanel.setPosition({ 1210.f, 150.f });
    m_sidePanel.setFillColor(sf::Color(16, 18, 28, 215));
    m_sidePanel.setOutlineColor(sf::Color(235, 200, 105));
    m_sidePanel.setOutlineThickness(2.f);

    m_resultPanel.setPosition({ 1240.f, 570.f });
    m_resultPanel.setFillColor(sf::Color(22, 28, 38, 235));
    m_resultPanel.setOutlineColor(sf::Color(255, 221, 122));
    m_resultPanel.setOutlineThickness(2.f);

    m_resultBadge.setPosition({ 1272.f, 620.f });
    m_resultBadge.setFillColor(sf::Color(15, 135, 65));
    m_resultBadge.setOutlineColor(sf::Color::White);
    m_resultBadge.setOutlineThickness(2.f);

    m_wheelMarker.setPointCount(3);
    m_wheelMarker.setPoint(0, { 0.f, 0.f });
    m_wheelMarker.setPoint(1, { 34.f, 0.f });
    m_wheelMarker.setPoint(2, { 17.f, 38.f });
    m_wheelMarker.setFillColor(sf::Color(255, 215, 0));
    m_wheelMarker.setOutlineColor(sf::Color::Black);
    m_wheelMarker.setOutlineThickness(2.f);
    m_wheelMarker.setPosition({ 1478.f, 94.f });

    m_title.setFillColor(sf::Color::White);
    {
        const sf::FloatRect bounds = m_title.getLocalBounds();
        m_title.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });
        m_title.setPosition({ 960.f, 34.f });
    }

    m_subtitle.setFillColor(sf::Color(255, 222, 150));
    {
        const sf::FloatRect bounds = m_subtitle.getLocalBounds();
        m_subtitle.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });
        m_subtitle.setPosition({ 960.f, 92.f });
    }

    m_playerNameText.setFillColor(sf::Color::White);
    m_playerNameText.setPosition({ 90.f, 54.f });

    m_balanceText.setFillColor(sf::Color::White);
    m_balanceText.setPosition({ 1560.f, 54.f });

    m_selectionTitle.setFillColor(sf::Color::White);
    m_selectionTitle.setPosition({ 100.f, 804.f });

    m_betAmountLabel.setFillColor(sf::Color::White);
    m_betAmountLabel.setPosition({ 160.f, 880.f });

    m_numberLabel.setFillColor(sf::Color::White);
    m_numberLabel.setPosition({ 520.f, 880.f });

    m_lastSpinTitle.setFillColor(sf::Color::White);
    m_lastSpinTitle.setPosition({ 1390.f, 592.f });

    m_lastSpinValue.setFillColor(sf::Color::White);
    m_lastSpinValue.setPosition({ 1390.f, 636.f });

    m_lastOutcomeText.setFillColor(sf::Color(225, 225, 225));
    m_lastOutcomeText.setPosition({ 160.f, 980.f });

    m_helpText.setFillColor(sf::Color(230, 230, 230));
    m_helpText.setPosition({ 858.f, 795.f });

    refreshPlayerStats();
    refreshStatus("Choose a bet type and press Spin.");
    refreshLastSpinTexts();
    refreshSelectionState();
}

void RouletteUI::refreshPlayerStats()
{
    m_playerNameText.setString("Player: " + m_player.getName());
    m_balanceText.setString("Balance: $" + formatMoney(m_player.getBalance()));

    const sf::FloatRect balanceBounds = m_balanceText.getLocalBounds();
    m_balanceText.setOrigin({
        balanceBounds.position.x + balanceBounds.size.x,
        0.f
        });
    m_balanceText.setPosition({ 1810.f, 54.f });
}

void RouletteUI::refreshSelectionState()
{
    m_numberButton.selected = (m_selectedBetType == Roulette::BetType::Number);
    m_redButton.selected = (m_selectedBetType == Roulette::BetType::Red);
    m_blackButton.selected = (m_selectedBetType == Roulette::BetType::Black);
    m_evenButton.selected = (m_selectedBetType == Roulette::BetType::Even);
    m_oddButton.selected = (m_selectedBetType == Roulette::BetType::Odd);

    const bool numberSelected = (m_selectedBetType == Roulette::BetType::Number);

    if (!numberSelected)
    {
        m_numberInput.box.setFillColor(sf::Color(30, 30, 38, 190));
        if (m_activeField != ActiveField::Number)
            m_numberInput.setActive(false);
    }
    else
    {
        m_numberInput.box.setFillColor(sf::Color(20, 20, 30, 220));
    }

    const bool allowActions = !m_spinning;
    m_spinButton.enabled = allowActions;
    m_clearButton.enabled = allowActions;
    m_returnButton.enabled = allowActions;
}

void RouletteUI::refreshStatus(const std::string& text)
{
    m_status.setString(text);
    m_status.setFillColor(sf::Color::White);

    const sf::FloatRect bounds = m_status.getLocalBounds();
    m_status.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y
        });
    m_status.setPosition({ 960.f, 128.f });
}

void RouletteUI::refreshLastSpinTexts()
{
    if (!m_hasLastOutcome)
    {
        m_lastSpinValue.setString("--");
        m_lastSpinValue.setFillColor(sf::Color::White);
        m_lastOutcomeText.setString("Place a wager to begin.");
        m_resultBadge.setFillColor(sf::Color(15, 135, 65));
        return;
    }

    m_lastSpinValue.setString(
        std::to_string(m_lastOutcome.winningNumber) +
        " - " + Roulette::pocketColorToString(m_lastOutcome.winningColor));
    m_lastSpinValue.setFillColor(getPocketDisplayColor(m_lastOutcome.winningColor));
    m_resultBadge.setFillColor(getPocketDisplayColor(m_lastOutcome.winningColor));

    std::string details = m_lastOutcome.message;
    details += "  Net: ";
    if (m_lastOutcome.netChange >= 0.0)
        details += "+$";
    else
        details += "-$";
    details += formatMoney(std::fabs(m_lastOutcome.netChange));

    if (m_lastOutcome.win && m_lastOutcome.payout > 0.0)
        details += "  Return: $" + formatMoney(m_lastOutcome.payout);

    m_lastOutcomeText.setString(details);
}

void RouletteUI::setBetType(Roulette::BetType type)
{
    if (m_spinning)
        return;

    m_selectedBetType = type;

    if (type != Roulette::BetType::Number)
    {
        m_activeField = ActiveField::None;
        m_numberInput.setActive(false);
    }

    m_audio.playSound("roulette_chip", 75.f);
    refreshSelectionState();
}

void RouletteUI::clearRoundInputs()
{
    if (m_spinning)
        return;

    m_betAmountInput.value.clear();
    m_numberInput.value.clear();

    m_betAmountInput.syncText();
    m_numberInput.syncText();

    m_activeField = ActiveField::None;
    m_betAmountInput.setActive(false);
    m_numberInput.setActive(false);

    refreshStatus("Fields cleared.");
}

void RouletteUI::startSpinAnimation(const Roulette::RoundOutcome& outcome)
{
    m_pendingOutcome = outcome;
    m_hasPendingOutcome = true;
    m_spinning = true;
    m_spinClock.restart();

    m_spinTargetAngle =
        m_spinStartAngle +
        1800.f +
        (360.f - getWheelStopOffset(outcome.winningNumber)) +
        kWheelVisualCalibrationDegrees;

    m_lastSpinValue.setString("...");
    m_lastSpinValue.setFillColor(sf::Color::White);
    m_lastOutcomeText.setString("Wheel is spinning...");
    refreshStatus("Spinning the wheel...");
    refreshSelectionState();
}

void RouletteUI::completeSpinAnimation()
{
    if (!m_hasPendingOutcome)
        return;

    m_spinning = false;
    m_wheelAngle = normalizeAngle(m_spinTargetAngle);
    m_wheelSprite.setRotation(sf::degrees(m_wheelAngle));

    m_lastOutcome = m_pendingOutcome;
    m_hasLastOutcome = true;
    m_hasPendingOutcome = false;

    m_player.recordGameResult("Roulette", m_lastOutcome.win);

    refreshLastSpinTexts();
    refreshPlayerStats();

    if (m_lastOutcome.win)
    {
        m_audio.playSound("win");
        refreshStatus("Winning spin!");
    }
    else
    {
        refreshStatus("No luck this spin.");
    }

    refreshSelectionState();
}

void RouletteUI::updateSpinAnimation()
{
    if (!m_spinning)
        return;

    const float elapsed = m_spinClock.getElapsedTime().asSeconds();
    const float t = std::clamp(elapsed / m_spinDurationSeconds, 0.f, 1.f);
    const float eased = easeOutCubic(t);

    m_wheelAngle =
        m_spinStartAngle +
        (m_spinTargetAngle - m_spinStartAngle) * eased;

    m_wheelSprite.setRotation(sf::degrees(m_wheelAngle));

    if (t >= 1.f)
        completeSpinAnimation();
}

void RouletteUI::handleBackspace()
{
    if (m_spinning)
        return;

    if (m_activeField == ActiveField::BetAmount && !m_betAmountInput.value.empty())
    {
        m_betAmountInput.value.pop_back();
        m_betAmountInput.syncText();
    }
    else if (m_activeField == ActiveField::Number && !m_numberInput.value.empty())
    {
        m_numberInput.value.pop_back();
        m_numberInput.syncText();
    }
}

void RouletteUI::handleTextEntered(const sf::Event& event)
{
    if (m_spinning || m_activeField == ActiveField::None)
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
    InputField* activeField = nullptr;

    if (m_activeField == ActiveField::BetAmount)
        activeField = &m_betAmountInput;
    else if (m_activeField == ActiveField::Number)
        activeField = &m_numberInput;

    if (activeField == nullptr)
        return;

    if (activeField->numericOnly)
    {
        const bool isDigit = character >= '0' && character <= '9';
        const bool isDecimal = character == '.' && activeField->decimalAllowed;

        if (!isDigit && !isDecimal)
            return;

        if (isDecimal && activeField->value.find('.') != std::string::npos)
            return;
    }

    if (activeField->value.size() >= 12)
        return;

    activeField->value.push_back(character);
    activeField->syncText();
}

bool RouletteUI::tryParseBetAmount(double& amount) const
{
    return tryParseRangedValue(m_betAmountInput.value, amount, 0.01, m_player.getBalance());
}

bool RouletteUI::tryParseBetNumber(int& number) const
{
    return tryParseRangedValue(m_numberInput.value, number, 0, 36);
}

Roulette::BetRequest RouletteUI::buildBetFromInputs(bool& valid) const
{
    valid = false;

    Roulette::BetRequest bet{};
    bet.type = m_selectedBetType;
    bet.chosenNumber = -1;

    if (!tryParseBetAmount(bet.amount))
        return bet;

    if (m_selectedBetType == Roulette::BetType::Number)
    {
        if (!tryParseBetNumber(bet.chosenNumber))
            return bet;
    }

    valid = true;
    return bet;
}

void RouletteUI::handleMouseClick(const sf::Event& event, sf::RenderWindow& window)
{
    if (m_returnButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        m_running = false;
        return;
    }

    if (m_numberButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        setBetType(Roulette::BetType::Number);
        return;
    }

    if (m_redButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        setBetType(Roulette::BetType::Red);
        return;
    }

    if (m_blackButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        setBetType(Roulette::BetType::Black);
        return;
    }

    if (m_evenButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        setBetType(Roulette::BetType::Even);
        return;
    }

    if (m_oddButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        setBetType(Roulette::BetType::Odd);
        return;
    }

    if (m_clearButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        clearRoundInputs();
        return;
    }

    if (m_spinButton.isClicked(event, window))
    {
        m_audio.playSound("button");

        bool betValid = false;
        const Roulette::BetRequest bet = buildBetFromInputs(betValid);

        if (!betValid)
        {
            if (m_selectedBetType == Roulette::BetType::Number && m_numberInput.value.empty())
                refreshStatus("Enter a number from 0 to 36.");
            else
                refreshStatus("Enter a valid wager before spinning.");
            return;
        }

        const Roulette::RoundOutcome outcome = m_roulette.playRound(m_player, bet);
        if (!outcome.success)
        {
            refreshStatus(outcome.message.empty()
                ? "That wager is not valid for your balance."
                : outcome.message);
            return;
        }

        m_audio.playSound("roulette_spin", 85.f);
        startSpinAnimation(outcome);
        return;
    }

    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePressed->button != sf::Mouse::Button::Left)
            return;

        const sf::Vector2f mouseCoords =
            window.mapPixelToCoords(mousePressed->position, window.getDefaultView());

        const bool clickedBetAmount = m_betAmountInput.contains(mouseCoords);
        const bool clickedNumber =
            m_numberInput.contains(mouseCoords)
            && m_selectedBetType == Roulette::BetType::Number;

        if (clickedBetAmount)
        {
            m_activeField = ActiveField::BetAmount;
            m_betAmountInput.setActive(true);
            m_numberInput.setActive(false);
        }
        else if (clickedNumber)
        {
            m_activeField = ActiveField::Number;
            m_betAmountInput.setActive(false);
            m_numberInput.setActive(true);
        }
        else
        {
            m_activeField = ActiveField::None;
            m_betAmountInput.setActive(false);
            m_numberInput.setActive(false);
        }
    }
}

void RouletteUI::draw(sf::RenderWindow& window)
{
    if (m_backgroundTexture.getSize().x > 0)
        window.draw(m_backgroundSprite);
    else
    {
        sf::RectangleShape backdrop({ kWindowWidth, kWindowHeight });
        backdrop.setFillColor(sf::Color(10, 18, 22));
        window.draw(backdrop);
    }

    if (m_tableTexture.getSize().x > 0)
        window.draw(m_tableSprite);
    else
        window.draw(m_tableFallback);

    window.draw(m_topBar);
    window.draw(m_sidePanel);
    window.draw(m_resultPanel);
    window.draw(m_bottomPanel);

    if (m_wheelTexture.getSize().x > 0)
        window.draw(m_wheelSprite);
    else
        window.draw(m_wheelFallback);

    window.draw(m_wheelMarker);
    window.draw(m_resultBadge);

    window.draw(m_title);
    window.draw(m_subtitle);
    window.draw(m_status);
    window.draw(m_playerNameText);
    window.draw(m_balanceText);

    window.draw(m_selectionTitle);
    window.draw(m_betAmountLabel);
    window.draw(m_numberLabel);
    window.draw(m_lastSpinTitle);
    window.draw(m_lastSpinValue);
    window.draw(m_lastOutcomeText);
    window.draw(m_helpText);

    m_numberButton.draw(window);
    m_redButton.draw(window);
    m_blackButton.draw(window);
    m_evenButton.draw(window);
    m_oddButton.draw(window);

    m_betAmountInput.draw(window);
    m_numberInput.draw(window);

    m_spinButton.draw(window);
    m_clearButton.draw(window);
    m_returnButton.draw(window);
}

void RouletteUI::run(sf::RenderWindow& window)
{
    m_audio.playMusicFadeIn("assets/sound/background/roulette_track.wav", 1.25f, true, 15.f);

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

        updateSpinAnimation();
        refreshSelectionState();

        m_numberButton.update(window);
        m_redButton.update(window);
        m_blackButton.update(window);
        m_evenButton.update(window);
        m_oddButton.update(window);
        m_spinButton.update(window);
        m_clearButton.update(window);
        m_returnButton.update(window);

        window.clear();
        draw(window);
        window.display();
    }

    if (window.isOpen())
        m_audio.playMusic("assets/sound/background/main_track.wav", true, 15.f);
}
