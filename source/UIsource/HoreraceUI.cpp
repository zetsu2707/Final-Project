// Description: Source file for the UI of Horse Racing minigame/Horse Racing game class.
// Related Files: HorseRaceUI.h, Audio.h, Player.h
// Date Created: 4/10/2026
// Last Edited: 4/11/2026

#include "UIheaders/HorseRaceUI.h"
#include "Audio.h"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <string>

HorseRaceUI::HorseRaceUI(sf::RenderWindow& window, sf::Font& font, AudioManager& audio)
    : m_window(window),
    m_font(font),
    m_audio(audio),
    m_view(sf::FloatRect({ 0.0f, 0.0f }, { BaseWidth, BaseHeight }))
{
    setupLanes();
    loadAssets();
    resetVisualState();
}

void HorseRaceUI::open(Player& player)
{
    m_player = &player;
    m_screenState = ScreenState::Betting;
    m_selectedHorse = 1;
    m_wantsToLeave = false;
    m_betFieldActive = false;
    m_betInputJustActivated = false;
    m_audio.playMusic("assets/sound/background/horserace_track.wav", true, 15.f);

    if (player.getBalance() <= 0.0)
    {
        m_betAmount = 0.0;
    }
    else if (player.getBalance() < MinimumBet)
    {
        m_betAmount = player.getBalance();
    }
    else
    {
        m_betAmount = std::min(10.0, player.getBalance());
    }

    setBetInputFromAmount(m_betAmount);
    resetVisualState();
}

void HorseRaceUI::updateView()
{
    const sf::Vector2u windowSize = m_window.getSize();
    if (windowSize.x == 0 || windowSize.y == 0)
    {
        return;
    }

    const float windowRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
    const float targetRatio = BaseWidth / BaseHeight;

    float viewportX = 0.0f;
    float viewportY = 0.0f;
    float viewportWidth = 1.0f;
    float viewportHeight = 1.0f;

    if (windowRatio > targetRatio)
    {
        viewportWidth = targetRatio / windowRatio;
        viewportX = (1.0f - viewportWidth) * 0.5f;
    }
    else if (windowRatio < targetRatio)
    {
        viewportHeight = windowRatio / targetRatio;
        viewportY = (1.0f - viewportHeight) * 0.5f;
    }

    m_view.setSize({ BaseWidth, BaseHeight });
    m_view.setCenter({ BaseWidth * 0.5f, BaseHeight * 0.5f });
    m_view.setViewport({ { viewportX, viewportY }, { viewportWidth, viewportHeight } });
}

void HorseRaceUI::handleEvent(const sf::Event& event)
{
    if (event.is<sf::Event::Resized>())
    {
        updateView();
        return;
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
        {
            m_wantsToLeave = true;
        }
    }
    else if (const auto* textEntered = event.getIf<sf::Event::TextEntered>())
    {
        if (m_screenState == ScreenState::Betting && m_betFieldActive)
        {
            handleTextEntered(textEntered->unicode);
        }
    }
    else if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePressed->button == sf::Mouse::Button::Left)
        {
            const sf::Vector2f mouseWorld =
                m_window.mapPixelToCoords(mousePressed->position, m_view);

            handleMousePressed(mouseWorld);
        }
    }
}

void HorseRaceUI::update(float deltaTime)
{
    updateView();

    if (m_screenState == ScreenState::Racing)
    {
        updateRaceAnimation(deltaTime);
    }

    syncHorseSprites();
}

void HorseRaceUI::draw()
{
    m_window.setView(m_view);

    drawBackground();
    drawTrack();
    drawHorseCards();
    drawHorses();
    drawBottomBar();

    if (m_screenState == ScreenState::Results)
    {
        drawResultsPanel();
    }
}

bool HorseRaceUI::wantsToLeave() const
{
    return m_wantsToLeave;
}

void HorseRaceUI::clearLeaveRequest()
{
    m_wantsToLeave = false;
}

void HorseRaceUI::loadAssets()
{
    m_hasFieldTexture = m_fieldTexture.loadFromFile("assets/horse_racing/field.jpg");
    if (m_hasFieldTexture)
    {
        m_fieldSprite.emplace(m_fieldTexture);

        const sf::Vector2u textureSize = m_fieldTexture.getSize();
        if (textureSize.x > 0 && textureSize.y > 0)
        {
            m_fieldSprite->setScale({
                BaseWidth / static_cast<float>(textureSize.x),
                BaseHeight / static_cast<float>(textureSize.y)
                });
        }
    }

    const std::array<std::string, HorseRacing::HorseCount> horsePaths = {
        "assets/horse_racing/horses/red.png",
        "assets/horse_racing/horses/blue.png",
        "assets/horse_racing/horses/green.png",
        "assets/horse_racing/horses/yellow.png",
        "assets/horse_racing/horses/black.png",
        "assets/horse_racing/horses/white.png",
        "assets/horse_racing/horses/purple.png",
        "assets/horse_racing/horses/orange.png"
    };

    for (int i = 0; i < HorseRacing::HorseCount; ++i)
    {
        m_hasHorseTexture[i] = m_horseTextures[i].loadFromFile(horsePaths[i]);

        if (m_hasHorseTexture[i])
        {
            m_horseSprites[i].emplace(m_horseTextures[i]);

            const sf::Vector2u textureSize = m_horseTextures[i].getSize();
            if (textureSize.x > 0 && textureSize.y > 0)
            {
                m_horseSprites[i]->setScale({
                    HorseDrawWidth / static_cast<float>(textureSize.x),
                    HorseDrawHeight / static_cast<float>(textureSize.y)
                    });
            }
        }
    }
}

void HorseRaceUI::setupLanes()
{
    for (int i = 0; i < HorseRacing::HorseCount; ++i)
    {
        const float laneTop = TrackTop + static_cast<float>(i) * (LaneHeight + LaneGap);
        m_laneY[i] = laneTop + (LaneHeight * 0.5f);
    }
}

void HorseRaceUI::resetVisualState()
{
    m_visualProgress.fill(0.0);
    m_phaseStartProgress.fill(0.0);
    m_phaseTargetProgress.fill(0.0);
    m_phaseAnimating = false;
    m_phaseAnimationTimer = 0.0f;
    m_betSettled = false;
    m_raceMaxProgress = 1.0;
    syncHorseSprites();
}

void HorseRaceUI::handleMousePressed(const sf::Vector2f& mousePosition)
{
    if (m_screenState == ScreenState::Betting)
    {
        handleBettingClick(mousePosition);
    }
    else if (m_screenState == ScreenState::Results)
    {
        handleResultsClick(mousePosition);
    }
}

void HorseRaceUI::handleBettingClick(const sf::Vector2f& mousePosition)
{
    for (int i = 0; i < HorseRacing::HorseCount; ++i)
    {
        if (getHorseCardBounds(i).contains(mousePosition))
        {
            m_audio.playSound("button");
            m_selectedHorse = i + 1;
            m_betFieldActive = false;
            m_betInputJustActivated = false;
            return;
        }
    }

    if (getBetInputBounds().contains(mousePosition))
    {
        m_audio.playSound("button");

        if (!m_betFieldActive)
        {
            m_betFieldActive = true;
            m_betInputJustActivated = true;
        }
        return;
    }

    m_betFieldActive = false;
    m_betInputJustActivated = false;

    if (getStartRaceButtonBounds().contains(mousePosition))
    {
        if (m_player == nullptr)
        {
            return;
        }

        if (!syncBetAmountFromInput())
        {
            return;
        }

        if (m_game.canPlaceBet(*m_player, m_betAmount))
        {
            m_audio.playSound("horseracestart");
            startRace();
        }

        return;
    }

    if (getBackButtonBounds().contains(mousePosition))
    {
        m_audio.playSound("button");
        m_audio.playMusic("assets/sound/background/main_track.wav", true, 15.f);
        m_wantsToLeave = true;
    }
}

void HorseRaceUI::handleResultsClick(const sf::Vector2f& mousePosition)
{
    if (getPlayAgainButtonBounds().contains(mousePosition))
    {
        m_audio.playSound("button");
        m_screenState = ScreenState::Betting;
        m_betFieldActive = false;
        m_betInputJustActivated = false;
        setBetInputFromAmount(m_betAmount);
        resetVisualState();
        return;
    }

    if (getBackButtonBounds().contains(mousePosition))
    {
        m_audio.playSound("button");
        m_wantsToLeave = true;
    }
}

void HorseRaceUI::handleTextEntered(char32_t unicode)
{
    if (unicode == 8)
    {
        if (!m_betInput.empty())
        {
            m_betInput.pop_back();
        }
        m_betInputJustActivated = false;
        return;
    }

    if (unicode == 13)
    {
        syncBetAmountFromInput();
        m_betFieldActive = false;
        m_betInputJustActivated = false;
        return;
    }

    if (unicode < 32 || unicode > 126)
    {
        return;
    }

    const char character = static_cast<char>(unicode);

    if (m_betInputJustActivated)
    {
        m_betInput.clear();
        m_betInputJustActivated = false;
    }

    if (character >= '0' && character <= '9')
    {
        if (m_betInput.size() < 10)
        {
            m_betInput.push_back(character);
        }
        return;
    }

    if (character == '.')
    {
        if (m_betInput.find('.') == std::string::npos)
        {
            if (m_betInput.empty())
            {
                m_betInput = "0.";
            }
            else
            {
                m_betInput.push_back(character);
            }
        }
    }
}

void HorseRaceUI::startRace()
{
    if (m_player == nullptr)
    {
        return;
    }

    resetVisualState();
    m_game.beginRace();

    m_raceMaxProgress = 1.0;
    for (const auto& horse : m_game.getHorses())
    {
        const double total = std::accumulate(
            horse.phaseScores.begin(),
            horse.phaseScores.end(),
            0.0
        );
        m_raceMaxProgress = std::max(m_raceMaxProgress, total);
    }

    m_screenState = ScreenState::Racing;
    beginNextPhaseAnimation();
}

void HorseRaceUI::beginNextPhaseAnimation()
{
    if (!m_game.isRaceStarted() || m_game.isRaceFinished())
    {
        return;
    }

    for (int i = 0; i < HorseRacing::HorseCount; ++i)
    {
        m_phaseStartProgress[i] = m_visualProgress[i];
    }

    if (!m_game.advanceRacePhase())
    {
        return;
    }

    for (int i = 0; i < HorseRacing::HorseCount; ++i)
    {
        m_phaseTargetProgress[i] = m_game.getHorseProgress(i);
    }

    m_phaseAnimating = true;
    m_phaseAnimationTimer = 0.0f;
}

void HorseRaceUI::updateRaceAnimation(float deltaTime)
{
    if (!m_phaseAnimating)
    {
        if (m_game.isRaceFinished())
        {
            settleBetIfNeeded();
            return;
        }

        beginNextPhaseAnimation();
        return;
    }

    m_phaseAnimationTimer += deltaTime;
    float t = m_phaseAnimationTimer / PhaseAnimationSeconds;
    t = std::clamp(t, 0.0f, 1.0f);

    for (int i = 0; i < HorseRacing::HorseCount; ++i)
    {
        const double start = m_phaseStartProgress[i];
        const double end = m_phaseTargetProgress[i];
        m_visualProgress[i] = start + (end - start) * static_cast<double>(t);
    }

    if (t >= 1.0f)
    {
        m_phaseAnimating = false;

        if (m_game.isRaceFinished())
        {
            settleBetIfNeeded();
        }
    }
}

void HorseRaceUI::settleBetIfNeeded()
{
    if (m_betSettled || m_player == nullptr)
    {
        return;
    }

    m_game.resolveBet(*m_player, m_betAmount, m_selectedHorse);

    if (m_game.didPlayerWinLastBet())
    {
        m_audio.playSound("win");
    }

    m_betSettled = true;
    m_screenState = ScreenState::Results;
}

void HorseRaceUI::syncHorseSprites()
{
    for (int i = 0; i < HorseRacing::HorseCount; ++i)
    {
        const double normalized = std::clamp(m_visualProgress[i] / m_raceMaxProgress, 0.0, 1.0);
        const float x = TrackLeft + static_cast<float>(normalized) * (TrackWidth - HorseDrawWidth);
        const float y = m_laneY[i] - (HorseDrawHeight * 0.5f);

        if (m_horseSprites[i].has_value())
        {
            m_horseSprites[i]->setPosition({ x, y });
        }
    }
}

void HorseRaceUI::setBetInputFromAmount(double amount)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << amount;
    m_betInput = stream.str();
}

bool HorseRaceUI::syncBetAmountFromInput()
{
    if (m_betInput.empty() || m_betInput == ".")
    {
        return false;
    }

    try
    {
        const double parsed = std::stod(m_betInput);
        if (parsed <= 0.0)
        {
            return false;
        }

        m_betAmount = parsed;
        setBetInputFromAmount(m_betAmount);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void HorseRaceUI::drawBackground()
{
    if (m_fieldSprite.has_value())
    {
        m_window.draw(*m_fieldSprite);
        return;
    }

    sf::RectangleShape background({ BaseWidth, BaseHeight });
    background.setFillColor(sf::Color(40, 140, 60));
    m_window.draw(background);
}

void HorseRaceUI::drawTrack()
{
    const float totalTrackHeight =
        (LaneHeight * static_cast<float>(HorseRacing::HorseCount)) +
        (LaneGap * static_cast<float>(HorseRacing::HorseCount - 1));

    sf::RectangleShape trackBase({ TrackWidth + 20.0f, totalTrackHeight + 20.0f });
    trackBase.setPosition({ TrackLeft - 10.0f, TrackTop - 10.0f });
    trackBase.setFillColor(sf::Color(120, 120, 80, 110));
    m_window.draw(trackBase);

    for (int i = 0; i < HorseRacing::HorseCount; ++i)
    {
        const float laneTop = TrackTop + static_cast<float>(i) * (LaneHeight + LaneGap);

        sf::RectangleShape lane({ TrackWidth, LaneHeight });
        lane.setPosition({ TrackLeft, laneTop });
        lane.setFillColor(sf::Color(235, 231, 193, 220));
        m_window.draw(lane);

        sf::RectangleShape border({ TrackWidth, 2.0f });
        border.setPosition({ TrackLeft, laneTop + LaneHeight - 2.0f });
        border.setFillColor(sf::Color(120, 120, 80));
        m_window.draw(border);
    }

    sf::RectangleShape startLine({ 5.0f, totalTrackHeight });
    startLine.setPosition({ TrackLeft - 8.0f, TrackTop });
    startLine.setFillColor(sf::Color::White);
    m_window.draw(startLine);

    sf::RectangleShape finishLine({ 6.0f, totalTrackHeight });
    finishLine.setPosition({ TrackRight, TrackTop });
    finishLine.setFillColor(sf::Color::Red);
    m_window.draw(finishLine);

    drawText("START", 24, { TrackLeft - 25.0f, TrackTop - 38.0f }, sf::Color::White);
    drawText("FINISH", 24, { TrackRight - 18.0f, TrackTop - 38.0f }, sf::Color::White);
}

void HorseRaceUI::drawHorseCards()
{
    const auto& horses = m_game.getHorses();

    for (int i = 0; i < HorseRacing::HorseCount; ++i)
    {
        const sf::FloatRect bounds = getHorseCardBounds(i);

        sf::RectangleShape card(bounds.size);
        card.setPosition(bounds.position);
        card.setFillColor(i + 1 == m_selectedHorse
            ? sf::Color(45, 45, 45, 220)
            : sf::Color(25, 25, 25, 205));
        card.setOutlineThickness(3.0f);
        card.setOutlineColor(i + 1 == m_selectedHorse ? sf::Color::Yellow : sf::Color(210, 210, 210));
        m_window.draw(card);

        std::ostringstream line1;
        line1 << (i + 1) << ". " << horses[i].color;

        std::ostringstream line2;
        line2 << "Payout x" << std::fixed << std::setprecision(2) << horses[i].payoutMultiplier;

        drawText(line1.str(), 22, { bounds.position.x + 12.0f, bounds.position.y + 12.0f });
        drawText(line2.str(), 18, { bounds.position.x + 12.0f, bounds.position.y + 44.0f }, sf::Color(220, 220, 220));
    }
}

void HorseRaceUI::drawHorses()
{
    const auto& horses = m_game.getHorses();

    for (int i = 0; i < HorseRacing::HorseCount; ++i)
    {
        if (m_horseSprites[i].has_value())
        {
            m_window.draw(*m_horseSprites[i]);
        }
        else
        {
            const double normalized = std::clamp(m_visualProgress[i] / m_raceMaxProgress, 0.0, 1.0);
            const float x = TrackLeft + static_cast<float>(normalized) * (TrackWidth - HorseDrawWidth);
            const float y = m_laneY[i] - (HorseDrawHeight * 0.5f);

            sf::RectangleShape fallback({ HorseDrawWidth, HorseDrawHeight });
            fallback.setPosition({ x, y });
            fallback.setFillColor(getHorseFallbackColor(horses[i].color));
            fallback.setOutlineThickness(2.0f);
            fallback.setOutlineColor(sf::Color::Black);
            m_window.draw(fallback);
        }
    }
}

void HorseRaceUI::drawBottomBar()
{
    const float barTop = 930.0f;

    sf::RectangleShape bar({ BaseWidth, 110.0f });
    bar.setPosition({ 0.0f, barTop });
    bar.setFillColor(sf::Color(18, 18, 18, 230));
    m_window.draw(bar);

    if (m_player != nullptr)
    {
        std::ostringstream balanceText;
        balanceText << "Balance: $" << std::fixed << std::setprecision(2) << m_player->getBalance();
        drawText(balanceText.str(), 30, { 25.0f, barTop + 32.0f });

        drawText("Bet:", 30, { 300.0f, barTop + 32.0f });

        std::ostringstream selectedText;
        selectedText << "Horse: " << m_game.getHorses()[m_selectedHorse - 1].color;
        drawText(selectedText.str(), 30, { 760.0f, barTop + 32.0f });
    }

    if (m_screenState == ScreenState::Betting)
    {
        const sf::FloatRect inputBounds = getBetInputBounds();

        sf::RectangleShape inputBox(inputBounds.size);
        inputBox.setPosition(inputBounds.position);
        inputBox.setFillColor(sf::Color(28, 28, 28));
        inputBox.setOutlineThickness(2.0f);
        inputBox.setOutlineColor(m_betFieldActive ? sf::Color::Yellow : sf::Color::White);
        m_window.draw(inputBox);

        drawText(
            m_betInput.empty() ? "0.00" : m_betInput,
            28,
            { inputBounds.position.x + 14.0f, inputBounds.position.y + 10.0f },
            sf::Color::White
        );

        bool canStart = false;
        if (m_player != nullptr)
        {
            try
            {
                if (!m_betInput.empty() && m_betInput != ".")
                {
                    const double previewBet = std::stod(m_betInput);
                    canStart = m_game.canPlaceBet(*m_player, previewBet);
                }
            }
            catch (...)
            {
                canStart = false;
            }
        }

        drawButton(getStartRaceButtonBounds(), "Start Race", canStart);
    }
    else if (m_screenState == ScreenState::Racing)
    {
        std::ostringstream phaseText;
        phaseText << "Phase " << m_game.getRaceState().currentPhase
            << " / " << HorseRacing::RacePhaseCount;
        drawText(phaseText.str(), 30, { 1180.0f, barTop + 32.0f });
    }
    else if (m_screenState == ScreenState::Results)
    {
        drawButton(getPlayAgainButtonBounds(), "Play Again", true);
    }

    drawButton(getBackButtonBounds(), "Back");
}

void HorseRaceUI::drawResultsPanel()
{
    sf::RectangleShape panel({ 360.0f, 240.0f });
    panel.setPosition({ 1510.0f, 90.0f });
    panel.setFillColor(sf::Color(15, 15, 15, 220));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color::White);
    m_window.draw(panel);

    drawText("Results", 30, { 1630.0f, 108.0f });

    const auto results = m_game.getRaceResults();

    for (int i = 0; i < 3 && i < static_cast<int>(results.size()); ++i)
    {
        std::ostringstream line;
        line << (i + 1) << ". " << results[i].color;
        drawText(line.str(), 24, { 1540.0f, 158.0f + static_cast<float>(i) * 36.0f });
    }

    if (m_game.didPlayerWinLastBet())
    {
        std::ostringstream payout;
        payout << "You won $" << std::fixed << std::setprecision(2) << m_game.getLastPayout();
        drawText(payout.str(), 24, { 1540.0f, 270.0f }, sf::Color(120, 255, 120));
    }
    else
    {
        drawText("You lost the bet.", 24, { 1540.0f, 270.0f }, sf::Color(255, 150, 150));
    }
}

sf::FloatRect HorseRaceUI::getHorseCardBounds(int horseIndex) const
{
    const float laneTop = TrackTop + static_cast<float>(horseIndex) * (LaneHeight + LaneGap);
    return { { 0.0f, laneTop }, { 205.0f, LaneHeight } };
}

sf::FloatRect HorseRaceUI::getBetInputBounds() const
{
    return { { 380.0f, 940.0f }, { 220.0f, 52.0f } };
}

sf::FloatRect HorseRaceUI::getStartRaceButtonBounds() const
{
    return { { 980.0f, 940.0f }, { 180.0f, 52.0f } };
}

sf::FloatRect HorseRaceUI::getBackButtonBounds() const
{
    return { { 1760.0f, 940.0f }, { 120.0f, 52.0f } };
}

sf::FloatRect HorseRaceUI::getPlayAgainButtonBounds() const
{
    return { { 1580.0f, 940.0f }, { 160.0f, 52.0f } };
}

void HorseRaceUI::drawButton(const sf::FloatRect& bounds, const std::string& label, bool highlighted)
{
    sf::RectangleShape button(bounds.size);
    button.setPosition(bounds.position);
    button.setFillColor(highlighted ? sf::Color(60, 110, 220) : sf::Color(45, 45, 45));
    button.setOutlineThickness(2.0f);
    button.setOutlineColor(sf::Color::White);
    m_window.draw(button);

    drawText(label, 24, { bounds.position.x + 16.0f, bounds.position.y + 10.0f });
}

void HorseRaceUI::drawText(const std::string& text,
    unsigned int size,
    const sf::Vector2f& position,
    const sf::Color& color)
{
    sf::Text drawable(m_font, text, size);
    drawable.setFillColor(color);
    drawable.setPosition(position);
    m_window.draw(drawable);
}

sf::Color HorseRaceUI::getHorseFallbackColor(const std::string& colorName)
{
    if (colorName == "Red") return sf::Color(220, 50, 50);
    if (colorName == "Blue") return sf::Color(60, 100, 255);
    if (colorName == "Green") return sf::Color(40, 180, 70);
    if (colorName == "Yellow") return sf::Color(240, 220, 50);
    if (colorName == "Black") return sf::Color(40, 40, 40);
    if (colorName == "White") return sf::Color(235, 235, 235);
    if (colorName == "Purple") return sf::Color(145, 70, 210);
    if (colorName == "Orange") return sf::Color(245, 135, 35);

    return sf::Color(180, 180, 180);
}