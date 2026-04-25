// Description: Implementation file for the Slots UI/controller class.
// Related Files: SlotsUI.h, Slots.h, Player.h, Audio.h, CasinoGameUI.cpp
// Date Created: 4/13/2026
// Last Edited: 4/13/2026

#include "UIheaders/SlotsUI.h"
#include "Audio.h"

#include <algorithm>
#include <iomanip>
#include <sstream>


SlotsUI::Button::Button(const sf::Font& font,
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
    const sf::FloatRect tb = text.getLocalBounds();
    text.setOrigin({ tb.position.x + tb.size.x / 2.f,
                     tb.position.y + tb.size.y / 2.f });
    const sf::FloatRect bb = box.getGlobalBounds();
    text.setPosition({ bb.position.x + bb.size.x / 2.f,
                       bb.position.y + bb.size.y / 2.f });
}

bool SlotsUI::Button::contains(sf::Vector2f p) const
{
    return box.getGlobalBounds().contains(p);
}

void SlotsUI::Button::update(const sf::RenderWindow& window)
{
    if (!enabled) {
        box.setFillColor(sf::Color(65, 65, 75, 180));
        text.setFillColor(sf::Color(180, 180, 180));
        return;
    }
    text.setFillColor(sf::Color::White);
    const sf::Vector2f mouse =
        window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());
    box.setFillColor(contains(mouse) ? sf::Color(90, 90, 130, 240)
                                     : sf::Color(40, 40, 60, 220));
}

bool SlotsUI::Button::isClicked(const sf::Event& event, const sf::RenderWindow& window) const
{
    if (!enabled) return false;
    if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>())
        if (mp->button == sf::Mouse::Button::Left)
            return contains(window.mapPixelToCoords(mp->position, window.getDefaultView()));
    return false;
}

void SlotsUI::Button::draw(sf::RenderWindow& window) const
{
    window.draw(box);
    window.draw(text);
}

SlotsUI::InputField::InputField(const sf::Font& font,
                                const std::string& placeholderText,
                                sf::Vector2f position,
                                sf::Vector2f size)
    : box(size), text(font, "", 28), placeholder(placeholderText)
{
    box.setPosition(position);
    box.setFillColor(sf::Color(25, 25, 35, 220));
    box.setOutlineColor(sf::Color(200, 200, 200));
    box.setOutlineThickness(2.f);
    text.setFillColor(sf::Color(210, 210, 210));
    text.setPosition({ position.x + 16.f, position.y + 12.f });
    syncText();
}

bool SlotsUI::InputField::contains(sf::Vector2f p) const
{
    return box.getGlobalBounds().contains(p);
}

void SlotsUI::InputField::setActive(bool active)
{
    box.setOutlineColor(active ? sf::Color(255, 215, 0) : sf::Color(200, 200, 200));
}

void SlotsUI::InputField::syncText()
{
    if (value.empty()) {
        text.setString(placeholder);
        text.setFillColor(sf::Color(170, 170, 170));
    } else {
        text.setString(value);
        text.setFillColor(sf::Color::White);
    }
}

void SlotsUI::InputField::draw(sf::RenderWindow& window) const
{
    window.draw(box);
    window.draw(text);
}


SlotsUI::SlotsUI(Player& player, AudioManager& audio)
    : m_player(player),
      m_audio(audio),
      m_slots(),
      m_running(true),
      m_betFieldActive(false),
      m_showPayouts(false),
      m_state(SlotState::Idle),
      m_lastMultiplier(0.0),
      m_lastBet(0.0),
      m_resultColor(sf::Color::White),
      m_spinTimer(0.f),
      m_font("assets/arial.ttf"),
      m_backgroundTexture("assets/game_selection_background.jpg"),
      m_backgroundSprite(m_backgroundTexture),
      m_title(m_font, "SLOT MACHINE", 58),
      m_balanceText(m_font, "", 30),
      m_statusText(m_font, "", 28),
      m_betInput(m_font, "Enter bet", { 120.f, 930.f }, { 280.f, 70.f }),
      m_spinButton(m_font, "Spin",     { 470.f, 930.f }, { 220.f, 70.f }),
      m_backButton(m_font, "Back",     { 1520.f, 930.f }, { 220.f, 70.f }),
      m_payoutsButton(m_font, "Payouts", { 760.f, 930.f }, { 220.f, 70.f })
{
    // Scale background to window
    const sf::Vector2u bgSize = m_backgroundTexture.getSize();
    if (bgSize.x > 0 && bgSize.y > 0)
        m_backgroundSprite.setScale({
            1920.f / static_cast<float>(bgSize.x),
            1080.f / static_cast<float>(bgSize.y) });

    // Title
    m_title.setFillColor(sf::Color::White);
    {
        const sf::FloatRect b = m_title.getLocalBounds();
        m_title.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y });
        m_title.setPosition({ 960.f, 22.f });
    }

    m_balanceText.setFillColor(sf::Color::White);
    m_balanceText.setPosition({ 80.f, 30.f });

    m_statusText.setFillColor(sf::Color::White);
    m_statusText.setPosition({ 960.f, 870.f });

    m_betInput.value = "10";
    m_betInput.syncText();

    // Load symbol textures in same order as Slots::initializeSymbols()
    const std::vector<std::string> texturePaths = {
        "assets/slots/slotsCherry.png",
        "assets/slots/slotsLemon.png",
        "assets/slots/slotsOrangeg.png",
        "assets/slots/slotsGrape.png",
        "assets/slots/slotsWatermelon.png",
        "assets/slots/slotsStar.png",
    };
    for (const auto& path : texturePaths)
        m_symbolTextures.emplace_back(path);

    refreshTexts();
    updateButtonStates();
}


double SlotsUI::getParsedBet() const
{
    try {
        if (m_betInput.value.empty()) return 0.0;
        return std::stod(m_betInput.value);
    } catch (...) { return 0.0; }
}

void SlotsUI::refreshTexts()
{
    {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2)
           << "Balance: $" << m_player.getBalance();
        m_balanceText.setString(ss.str());
    }

    // Centre the status text
    const sf::FloatRect sb = m_statusText.getLocalBounds();
    m_statusText.setOrigin({ sb.position.x + sb.size.x / 2.f, sb.position.y });
    m_statusText.setPosition({ 960.f, 870.f });
}

void SlotsUI::updateButtonStates()
{
    const bool idle = (m_state == SlotState::Idle || m_state == SlotState::Result);
    m_spinButton.enabled    = idle;
    m_backButton.enabled    = idle;
    m_payoutsButton.enabled = idle;
}


void SlotsUI::doSpin()
{
    const double bet = getParsedBet();
    if (bet <= 0.0) {
        m_statusText.setString("Enter a valid bet amount.");
        m_statusText.setFillColor(sf::Color(255, 180, 80));
        refreshTexts();
        return;
    }
    if (!m_player.placeBet(bet)) {
        m_statusText.setString("Insufficient funds.");
        m_statusText.setFillColor(sf::Color(255, 120, 120));
        refreshTexts();
        return;
    }

    m_lastBet    = bet;
    m_lastResult = m_slots.spinReels();
    m_state      = SlotState::Spinning;
    m_spinTimer  = 0.f;
    m_spinIndices = { 0, 2, 4 };  // staggered start per reel
    m_spinFrameClock.restart();

    refreshTexts();
    updateButtonStates();
}


void SlotsUI::handleBackspace()
{
    if (m_betFieldActive && !m_betInput.value.empty()) {
        m_betInput.value.pop_back();
        m_betInput.syncText();
    }
}

void SlotsUI::handleTextEntered(const sf::Event& event)
{
    if (!m_betFieldActive || m_state == SlotState::Spinning) return;

    const auto* te = event.getIf<sf::Event::TextEntered>();
    if (!te) return;

    const char32_t uni = te->unicode;
    if (uni == 8) { handleBackspace(); return; }
    if (uni < 32 || uni > 126) return;

    const char ch = static_cast<char>(uni);
    if ((ch >= '0' && ch <= '9') || ch == '.') {
        if (ch == '.' && m_betInput.value.find('.') != std::string::npos) return;
        if (m_betInput.value.size() < 10) {
            m_betInput.value.push_back(ch);
            m_betInput.syncText();
        }
    }
}

void SlotsUI::handleMouseClick(const sf::Event& event, sf::RenderWindow& window)
{
    if (m_backButton.isClicked(event, window)) {
        m_audio.playSound("button");
        m_running = false;
        return;
    }

    if (m_spinButton.isClicked(event, window)) {
        m_audio.playSound("button");
        doSpin();
        return;
    }

    if (m_payoutsButton.isClicked(event, window)) {
        m_audio.playSound("button");
        m_showPayouts = !m_showPayouts;
        return;
    }

    // Bet field click
    if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mp->button == sf::Mouse::Button::Left) {
            const sf::Vector2f coords =
                window.mapPixelToCoords(mp->position, window.getDefaultView());
            m_betFieldActive = m_betInput.contains(coords) && (m_state != SlotState::Spinning);
            m_betInput.setActive(m_betFieldActive);
        }
    }
}


void SlotsUI::drawReels(sf::RenderWindow& window) const
{
    const float reelW  = 200.f;
    const float reelH  = 240.f;
    const float gap    = 40.f;
    const float totalW = 3.f * reelW + 2.f * gap;
    const float startX = 960.f - totalW / 2.f;
    const float reelY  = 340.f;
    const float iconSz = 160.f;

    const auto& syms = m_slots.getSymbols();

    for (int i = 0; i < 3; ++i)
    {
        const float x = startX + static_cast<float>(i) * (reelW + gap);

        // Reel background
        sf::RectangleShape reel({ reelW, reelH });
        reel.setPosition({ x, reelY });
        reel.setFillColor(sf::Color(20, 20, 30, 230));
        reel.setOutlineColor(sf::Color(255, 215, 0));
        reel.setOutlineThickness(3.f);
        window.draw(reel);

        // Pick which texture to show
        std::size_t texIdx = m_spinIndices[static_cast<std::size_t>(i)];

        if (m_state != SlotState::Spinning && !m_lastResult.empty())
        {
            const std::string& disp = m_lastResult[static_cast<std::size_t>(i)].display;
            for (std::size_t j = 0; j < syms.size(); ++j)
            {
                if (syms[j].display == disp) { texIdx = j; break; }
            }
        }

        if (texIdx < m_symbolTextures.size())
        {
            const sf::Texture& tex = m_symbolTextures[texIdx];
            const sf::Vector2u sz  = tex.getSize();
            if (sz.x > 0 && sz.y > 0)
            {
                sf::Sprite icon(tex);
                icon.setScale({ iconSz / sz.x, iconSz / sz.y });
                icon.setPosition({
                    x + (reelW - iconSz) / 2.f,
                    reelY + (reelH - iconSz) / 2.f
                });
                window.draw(icon);
            }
        }
    }
}

void SlotsUI::drawPayoutTable(sf::RenderWindow& window) const
{
    if (!m_showPayouts) return;

    const float panelW = 460.f;
    const float panelH = 60.f + static_cast<float>(m_slots.getSymbols().size()) * 46.f + 60.f;
    const float panelX = 960.f - panelW / 2.f;
    const float panelY = 120.f;

    sf::RectangleShape panel({ panelW, panelH });
    panel.setPosition({ panelX, panelY });
    panel.setFillColor(sf::Color(15, 15, 20, 235));
    panel.setOutlineColor(sf::Color(255, 215, 0));
    panel.setOutlineThickness(2.f);
    window.draw(panel);

    sf::Text header(m_font, "-- Payout Table --", 30);
    header.setFillColor(sf::Color(255, 215, 0));
    {
        const sf::FloatRect b = header.getLocalBounds();
        header.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y });
    }
    header.setPosition({ 960.f, panelY + 12.f });
    window.draw(header);

    float y = panelY + 60.f;
    for (const auto& sym : m_slots.getSymbols()) {
        std::ostringstream line;
        line << sym.display
             << "  x3 = " << std::fixed << std::setprecision(1) << sym.multiplier
             << "x   |   x2 = 0.5x";

        sf::Text row(m_font, line.str(), 24);
        row.setFillColor(sf::Color::White);
        {
            const sf::FloatRect b = row.getLocalBounds();
            row.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y });
        }
        row.setPosition({ 960.f, y });
        window.draw(row);
        y += 46.f;
    }
}

void SlotsUI::drawResultBanner(sf::RenderWindow& window) const
{
    if (m_state != SlotState::Result || m_resultMessage.empty()) return;

    sf::Text banner(m_font, m_resultMessage, 38);
    banner.setFillColor(m_resultColor);
    {
        const sf::FloatRect b = banner.getLocalBounds();
        banner.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y });
    }
    banner.setPosition({ 960.f, 620.f });
    window.draw(banner);
}

void SlotsUI::draw(sf::RenderWindow& window)
{
    window.draw(m_backgroundSprite);

    // Dark overlay panel
    sf::RectangleShape overlay({ 1400.f, 780.f });
    overlay.setPosition({ 260.f, 80.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    overlay.setOutlineColor(sf::Color(255, 215, 0, 80));
    overlay.setOutlineThickness(2.f);
    window.draw(overlay);

    window.draw(m_title);
    window.draw(m_balanceText);

    // "Spinning…" label during animation
    if (m_state == SlotState::Spinning) {
        sf::Text spin(m_font, "Spinning...", 42);
        spin.setFillColor(sf::Color(255, 215, 0));
        const sf::FloatRect b = spin.getLocalBounds();
        spin.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y });
        spin.setPosition({ 960.f, 270.f });
        window.draw(spin);
    }

    drawReels(window);
    drawResultBanner(window);

    // Status
    window.draw(m_statusText);

    // Bottom bar background
    sf::RectangleShape bar({ 1920.f, 110.f });
    bar.setPosition({ 0.f, 920.f });
    bar.setFillColor(sf::Color(18, 18, 18, 230));
    window.draw(bar);

    m_betInput.draw(window);
    m_spinButton.draw(window);
    m_backButton.draw(window);
    m_payoutsButton.draw(window);

    if (m_showPayouts)
        drawPayoutTable(window);
}


void SlotsUI::run(sf::RenderWindow& window)
{
    m_audio.playMusicFadeIn("assets/sound/background/slots_track.wav", 1.5f, true, 15.f);

    sf::Clock frameClock;

    while (window.isOpen() && m_running)
    {
        m_audio.update();
        const float dt = frameClock.restart().asSeconds();

        // ── Spin animation tick ──────────────────────────────────────────
        if (m_state == SlotState::Spinning)
        {
            // Cycle each reel icon at ~12 fps, staggered by reel
            if (m_spinFrameClock.getElapsedTime().asSeconds() >= 0.085f)
            {
                const std::size_t n = m_symbolTextures.size();
                if (n > 0)
                {
                    m_spinIndices[0] = (m_spinIndices[0] + 1) % n;
                    m_spinIndices[1] = (m_spinIndices[1] + 1) % n;
                    m_spinIndices[2] = (m_spinIndices[2] + 1) % n;
                }
                m_spinFrameClock.restart();
            }

            m_spinTimer += dt;
            if (m_spinTimer >= SpinDuration)
            {
                // Resolve the spin result
                m_lastMultiplier = m_slots.evaluate(m_lastResult);

                if (m_lastMultiplier == 0.0) {
                    m_resultMessage = "No match - you lost $"
                        + [&]{ std::ostringstream ss;
                               ss << std::fixed << std::setprecision(2) << m_lastBet;
                               return ss.str(); }() + ".";
                    m_resultColor = sf::Color(255, 120, 120);
                } else if (m_lastMultiplier == 0.5) {
                    const double win = m_lastBet * 0.5;
                    m_player.addWinnings(win);
                    std::ostringstream ss;
                    ss << "Two of a kind! +" << std::fixed << std::setprecision(2) << win;
                    m_resultMessage = ss.str();
                    m_resultColor   = sf::Color(255, 220, 80);
                    m_audio.playSound("win");
                } else {
                    const double win = m_lastBet * m_lastMultiplier;
                    m_player.addWinnings(win);
                    std::ostringstream ss;
                    ss << "THREE OF A KIND! +" << std::fixed << std::setprecision(2) << win
                       << "   (" << m_lastResult[0].display << "  x" << m_lastMultiplier << ")";
                    m_resultMessage = ss.str();
                    m_resultColor   = sf::Color(100, 255, 140);

                    const std::string& sym = m_lastResult[0].display;
                    const bool isJackpot = (sym == "Grapes" ||
                                            sym == "Watermelon" ||
                                            sym == "Star");
                    m_audio.playSound(isJackpot ? "jackpot" : "win");
                }

                m_player.recordGameResult("Slots", m_lastMultiplier > 0.0);
                m_state = SlotState::Result;
                refreshTexts();
                updateButtonStates();
            }
        }

        // ── Events ──────────────────────────────────────────────────────
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) { window.close(); continue; }
            if (event->is<sf::Event::TextEntered>()) handleTextEntered(*event);
            handleMouseClick(*event, window);
        }

        refreshTexts();
        updateButtonStates();

        m_spinButton.update(window);
        m_backButton.update(window);
        m_payoutsButton.update(window);

        window.clear();
        draw(window);
        window.display();
    }

    if (window.isOpen())
        m_audio.playMusic("assets/sound/background/main_track.wav", true, 15.f);
}
