// Description: Implementation file for the CasinoGameUI game selection/controller class.
// Related Files: CasinoGameUI.h, Player.h, Audio.h, and all game UI headers
// Date Created: 4/7/2026
// Last Edited: 4/24/2026

#include "UIheaders/CasinoGameUI.h"
#include "UIheaders/SlotsUI.h"
#include "Blackjack.h"
#include "UIheaders/BlackjackUI.h"
#include "HorseRace.h"
#include "UIHeaders/HorseRaceUI.h"
#include "Roulette.h"
#include "UIheaders/RouletteUI.h"
#include "Poker.h"
#include "UIheaders/PokerUI.h"
#include "Audio.h"

#include <iomanip>
#include <memory>
#include <optional>
#include <sstream>

// ── File-scope state (screen + horse race UI) ──────────────────────────────────
enum class Screen { Menu, HorseRace };
Screen m_currentScreen = Screen::Menu;
std::unique_ptr<HorseRaceUI> m_horseRaceUI;

// ─────────────────────────────────────────────────────────────────────────────
// Button
// ─────────────────────────────────────────────────────────────────────────────
CasinoGameUI::Button::Button(const sf::Font& font,
    const std::string& label,
    sf::Vector2f position,
    sf::Vector2f size)
    : box(size), text(font, label, 30)
{
    box.setPosition(position);
    box.setFillColor(sf::Color(40, 40, 60, 220));
    box.setOutlineColor(sf::Color::White);
    box.setOutlineThickness(2.f);

    text.setFillColor(sf::Color::White);

    const sf::FloatRect tb = text.getLocalBounds();
    text.setOrigin({ tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f });

    const sf::FloatRect bb = box.getGlobalBounds();
    text.setPosition({ bb.position.x + bb.size.x / 2.f, bb.position.y + bb.size.y / 2.f });
}

void CasinoGameUI::Button::setLabel(const sf::Font& font, const std::string& label)
{
    text = sf::Text(font, label, 30);
    text.setFillColor(sf::Color::White);
    const sf::FloatRect tb = text.getLocalBounds();
    text.setOrigin({ tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f });
    const sf::FloatRect bb = box.getGlobalBounds();
    text.setPosition({ bb.position.x + bb.size.x / 2.f, bb.position.y + bb.size.y / 2.f });
}

bool CasinoGameUI::Button::contains(sf::Vector2f point) const
{
    return box.getGlobalBounds().contains(point);
}

void CasinoGameUI::Button::update(const sf::RenderWindow& window)
{
    const sf::Vector2f mouseCoords =
        window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());
    box.setFillColor(contains(mouseCoords)
        ? sf::Color(90, 90, 130, 240)
        : sf::Color(40, 40, 60, 220));
}

bool CasinoGameUI::Button::isClicked(const sf::Event& event, const sf::RenderWindow& window) const
{
    if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>())
        if (mp->button == sf::Mouse::Button::Left)
            return contains(window.mapPixelToCoords(mp->position, window.getDefaultView()));
    return false;
}

void CasinoGameUI::Button::draw(sf::RenderWindow& window) const
{
    window.draw(box);
    window.draw(text);
}

// ─────────────────────────────────────────────────────────────────────────────
// ImageCard
// ─────────────────────────────────────────────────────────────────────────────
CasinoGameUI::ImageCard::ImageCard(const std::string& texturePath,
    sf::Vector2f position,
    sf::Vector2f size)
    : texture(texturePath), sprite(texture), hoverOutline(size)
{
    const sf::Vector2u ts = texture.getSize();
    sprite.setScale({ size.x / static_cast<float>(ts.x), size.y / static_cast<float>(ts.y) });
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
    const sf::Vector2f mc =
        window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());
    hoverOutline.setOutlineColor(contains(mc)
        ? sf::Color(255, 215, 0) : sf::Color::Transparent);
}

bool CasinoGameUI::ImageCard::isClicked(const sf::Event& event, const sf::RenderWindow& window) const
{
    if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>())
        if (mp->button == sf::Mouse::Button::Left)
            return contains(window.mapPixelToCoords(mp->position, window.getDefaultView()));
    return false;
}

void CasinoGameUI::ImageCard::draw(sf::RenderWindow& window) const
{
    window.draw(sprite);
    window.draw(hoverOutline);
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────
CasinoGameUI::CasinoGameUI(Player& player, AudioManager& audio, int currentSlot)
    : m_player(player),
    m_audio(audio),
    m_currentSlot(currentSlot),
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
    m_endTitle(m_font, "", 100),
    m_endSubtitle(m_font, "", 46),
    m_endStats(m_font, "", 34),
    m_statsOverlayText(m_font, "", 26),
    m_horseRaceCard("assets/horse_race_card.png",  {  80.f, 270.f }, { 500.f, 220.f }),
    m_blackjackCard ("assets/blackjack_card.png",  {1330.f, 270.f }, { 500.f, 220.f }),
    m_rouletteCard  ("assets/roulette_card.png",   { 390.f, 540.f }, { 500.f, 220.f }),
    m_pokerCard     ("assets/poker_card.png",       {1030.f, 540.f }, { 500.f, 220.f }),
    m_slotsCard     ("assets/slots_card.png",       { 710.f, 270.f }, { 500.f, 220.f }),
    m_saveGameButton (m_font, "Save Game",            { 200.f, 870.f }, { 320.f, 80.f }),
    m_statsButton    (m_font, "Statistics",           { 560.f, 870.f }, { 300.f, 80.f }),
    m_returnButton   (m_font, "Return to Main Menu",  { 900.f, 870.f }, { 420.f, 80.f }),
    m_endMenuButton  (m_font, "Return to Main Menu",  { 660.f, 750.f }, { 600.f, 90.f }),
    m_saveCancelButton  (m_font, "Cancel",            { 610.f, 910.f }, { 300.f, 70.f }),
    m_saveNoSaveButton  (m_font, "Return Without Saving", { 960.f, 910.f }, { 380.f, 70.f }),
    m_statsPanelClose   (m_font, "Close",            {1400.f, 175.f }, { 160.f, 60.f })
{
    const sf::Vector2u bgSize = m_backgroundTexture.getSize();
    m_backgroundSprite.setScale({
        1920.f / static_cast<float>(bgSize.x),
        1080.f / static_cast<float>(bgSize.y) });

    {
        const sf::Vector2u s = m_namePanelTexture.getSize();
        m_namePanelSprite.setScale({ 540.f / static_cast<float>(s.x), 210.f / static_cast<float>(s.y) });
        m_namePanelSprite.setPosition({ 310.f, 60.f });
    }
    {
        const sf::Vector2u s = m_balancePanelTexture.getSize();
        m_balancePanelSprite.setScale({ 540.f / static_cast<float>(s.x), 210.f / static_cast<float>(s.y) });
        m_balancePanelSprite.setPosition({ 1070.f, 60.f });
    }

    m_title.setFillColor(sf::Color::White);
    const sf::FloatRect titleBounds = m_title.getLocalBounds();
    m_title.setOrigin({ titleBounds.position.x + titleBounds.size.x / 2.f, titleBounds.position.y });
    m_title.setPosition({ 960.f, 30.f });

    m_status.setFillColor(sf::Color::White);
    m_status.setPosition({ 60.f, 20.f });

    m_playerNameText.setFillColor(sf::Color::White);
    m_playerNameText.setPosition({ 480.f, 150.f });

    m_balanceText.setFillColor(sf::Color::White);
    m_balanceText.setPosition({ 1250.f, 150.f });

    m_endTitle.setFillColor(sf::Color::White);
    m_endSubtitle.setFillColor(sf::Color::White);
    m_endStats.setFillColor(sf::Color::White);

    m_statsOverlayText.setFillColor(sf::Color(220, 220, 220));

    // Build 10 save-slot buttons (2 columns × 5 rows)
    for (int i = 0; i < 10; ++i)
    {
        const float col = (i < 5) ? 110.f : 1010.f;
        const float row = 190.f + (i % 5) * 130.f;
        m_saveSlotButtons.emplace_back(m_font, "...", sf::Vector2f{ col, row }, sf::Vector2f{ 750.f, 110.f });
    }

    refreshPlayerStats();
}

// ─────────────────────────────────────────────────────────────────────────────
// refreshSaveSlotButtons
// ─────────────────────────────────────────────────────────────────────────────
void CasinoGameUI::refreshSaveSlotButtons()
{
    for (int i = 0; i < 10; ++i)
    {
        Player temp("", 0);
        std::string label;
        if (temp.loadFromFile(Player::getSlotFilename(i + 1)))
        {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2);
            ss << "Slot " << (i + 1) << ":  " << temp.getName()
               << "   $" << temp.getBalance()
               << "   [" << temp.getGamesPlayed() << " games]";
            label = ss.str();
        }
        else
        {
            label = "Slot " + std::to_string(i + 1) + ":  [Empty]";
        }
        m_saveSlotButtons[i].setLabel(m_font, label);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// buildStatsText
// ─────────────────────────────────────────────────────────────────────────────
void CasinoGameUI::buildStatsText()
{
    const int played  = m_player.getGamesPlayed();
    const int won     = m_player.getGamesWon();
    const double winPct = played > 0 ? 100.0 * won / played : 0.0;

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "=== " << m_player.getName() << " ===\n\n";
    ss << "Balance:          $" << m_player.getBalance()     << "\n";
    ss << "Total Wagered:    $" << m_player.getTotalBetted() << "\n";
    ss << "Total Returned:   $" << m_player.getTotalWon()    << "\n";
    ss << "Net Profit:       $" << m_player.getNetProfit()   << "\n";
    ss << "Games Played:     "  << played;
    if (played > 0)
    {
        ss << "   Won: " << won << "   Lost: " << m_player.getGamesLost();
        ss << std::setprecision(1) << "   (" << winPct << "% win rate)";
    }
    ss << "\n\n" << std::setprecision(2);

    ss << "Per-Game Breakdown:\n";
    const auto& stats = m_player.getGameStats();
    if (stats.empty())
    {
        ss << "  No game history recorded yet.\n";
    }
    else
    {
        for (const auto& [name, stat] : stats)
        {
            const double net = stat.netEarnings;
            ss << "  " << name << ":   " << stat.played << " played,  "
               << stat.won << " won  |  Net: "
               << (net >= 0.0 ? "+" : "") << "$" << net << "\n";
        }
    }

    ss << "\nHighest Earning Game:  " << m_player.getHighestEarningGame() << "\n";
    ss << "Most Wins Game:        " << m_player.getMostWinsGame();

    m_statsOverlayText.setString(ss.str());
}

// ─────────────────────────────────────────────────────────────────────────────
// checkEndCondition
// ─────────────────────────────────────────────────────────────────────────────
void CasinoGameUI::checkEndCondition()
{
    const double balance = m_player.getBalance();
    if (balance >= 5000.0)
    {
        m_playerWon     = true;
        m_showEndScreen = true;
        m_audio.playSound("hakariwin");
    }
    else if (balance <= 0.0)
    {
        m_playerWon     = false;
        m_showEndScreen = true;
        m_audio.playSound("lose");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// drawEndScreen
// ─────────────────────────────────────────────────────────────────────────────
void CasinoGameUI::drawEndScreen(sf::RenderWindow& window)
{
    sf::RectangleShape overlay({ 1920.f, 1080.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(m_backgroundSprite);
    window.draw(overlay);

    if (m_playerWon)
    {
        m_endTitle.setString("YOU WIN!");
        m_endTitle.setFillColor(sf::Color(255, 215, 0));
        m_endSubtitle.setString("Congratulations! You reached $5,000!");
        m_endSubtitle.setFillColor(sf::Color(255, 255, 180));
    }
    else
    {
        m_endTitle.setString("GAME OVER");
        m_endTitle.setFillColor(sf::Color(220, 50, 50));
        m_endSubtitle.setString("You've run out of money. Better luck next time!");
        m_endSubtitle.setFillColor(sf::Color(255, 180, 180));
    }

    std::ostringstream stats;
    stats << std::fixed << std::setprecision(2);
    stats << "Player: "        << m_player.getName()       << "\n";
    stats << "Final Balance:  $" << m_player.getBalance()  << "\n";
    stats << "Total Wagered:  $" << m_player.getTotalBetted() << "\n";
    stats << "Total Won:      $" << m_player.getTotalWon()  << "\n";
    stats << "Net Profit:     $" << m_player.getNetProfit();
    m_endStats.setString(stats.str());
    m_endStats.setFillColor(sf::Color(220, 220, 220));

    auto centre = [&](sf::Text& t, float y)
    {
        const sf::FloatRect b = t.getLocalBounds();
        t.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f });
        t.setPosition({ 960.f, y });
    };

    centre(m_endTitle,    300.f);
    centre(m_endSubtitle, 430.f);
    centre(m_endStats,    570.f);

    window.draw(m_endTitle);
    window.draw(m_endSubtitle);
    window.draw(m_endStats);
    m_endMenuButton.draw(window);
}

// ─────────────────────────────────────────────────────────────────────────────
// drawSaveSlotMenu
// ─────────────────────────────────────────────────────────────────────────────
void CasinoGameUI::drawSaveSlotMenu(sf::RenderWindow& window)
{
    sf::RectangleShape overlay({ 1920.f, 1080.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 210));
    window.draw(overlay);

    sf::Text title(m_font,
        m_pendingReturn ? "SAVE BEFORE RETURNING?" : "SELECT A SAVE SLOT", 50);
    title.setFillColor(sf::Color(255, 215, 0));
    const sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin({ tb.position.x + tb.size.x / 2.f, tb.position.y });
    title.setPosition({ 960.f, 100.f });
    window.draw(title);

    for (auto& btn : m_saveSlotButtons)
        btn.draw(window);

    m_saveCancelButton.draw(window);
    if (m_pendingReturn)
        m_saveNoSaveButton.draw(window);
}

// ─────────────────────────────────────────────────────────────────────────────
// drawStatsPanel
// ─────────────────────────────────────────────────────────────────────────────
void CasinoGameUI::drawStatsPanel(sf::RenderWindow& window)
{
    sf::RectangleShape overlay({ 1920.f, 1080.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);

    sf::RectangleShape panel({ 1200.f, 720.f });
    panel.setPosition({ 360.f, 140.f });
    panel.setFillColor(sf::Color(20, 25, 40, 245));
    panel.setOutlineColor(sf::Color(255, 215, 0));
    panel.setOutlineThickness(2.f);
    window.draw(panel);

    m_statsOverlayText.setPosition({ 390.f, 160.f });
    window.draw(m_statsOverlayText);

    m_statsPanelClose.draw(window);
}

// ─────────────────────────────────────────────────────────────────────────────
// refreshPlayerStats
// ─────────────────────────────────────────────────────────────────────────────
void CasinoGameUI::refreshPlayerStats()
{
    m_playerNameText.setString(m_player.getName());

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << "$" << m_player.getBalance();
    m_balanceText.setString(ss.str());
}

// ─────────────────────────────────────────────────────────────────────────────
// handleMouseClick
// ─────────────────────────────────────────────────────────────────────────────
void CasinoGameUI::handleMouseClick(const sf::Event& event, sf::RenderWindow& window)
{
    // ── End screen ────────────────────────────────────────────────────────────
    if (m_showEndScreen)
    {
        if (m_endMenuButton.isClicked(event, window))
        {
            m_audio.playSound("button");
            m_running = false;
        }
        return;
    }

    // ── Stats panel ───────────────────────────────────────────────────────────
    if (m_showStatsPanel)
    {
        if (m_statsPanelClose.isClicked(event, window))
        {
            m_audio.playSound("button");
            m_showStatsPanel = false;
        }
        return;
    }

    // ── Save slot menu ────────────────────────────────────────────────────────
    if (m_showSaveMenu)
    {
        for (int i = 0; i < 10; ++i)
        {
            if (m_saveSlotButtons[i].isClicked(event, window))
            {
                m_audio.playSound("button");
                if (m_player.saveToFile(Player::getSlotFilename(i + 1)))
                {
                    m_currentSlot = i + 1;
                    m_showSaveMenu = false;
                    m_status.setString("Saved to Slot " + std::to_string(i + 1) + ".");
                    if (m_pendingReturn)
                    {
                        m_pendingReturn = false;
                        m_running = false;
                    }
                }
                else
                {
                    m_status.setString("Failed to save to Slot " + std::to_string(i + 1) + ".");
                }
                return;
            }
        }

        if (m_pendingReturn && m_saveNoSaveButton.isClicked(event, window))
        {
            m_audio.playSound("button");
            m_showSaveMenu  = false;
            m_pendingReturn = false;
            m_running       = false;
            return;
        }

        if (m_saveCancelButton.isClicked(event, window))
        {
            m_audio.playSound("button");
            m_showSaveMenu  = false;
            m_pendingReturn = false;
        }
        return;
    }

    // ── Horse Race card ───────────────────────────────────────────────────────
    if (m_horseRaceCard.isClicked(event, window))
    {
        m_audio.playSound("button");
        if (!m_horseRaceUI)
            m_horseRaceUI = std::make_unique<HorseRaceUI>(window, m_font, m_audio);
        m_horseRaceBalanceBefore = m_player.getBalance();
        m_horseRaceUI->open(m_player);
        m_currentScreen = Screen::HorseRace;
        m_status.setString("Horse Race loaded.");
        return;
    }

    // ── Game cards ────────────────────────────────────────────────────────────
    if (m_blackjackCard.isClicked(event, window))
    {
        m_audio.playSound("button");
        const double before = m_player.getBalance();
        BlackjackUI blackjackUI(m_player, m_audio);
        blackjackUI.run(window);
        m_player.updateGameEarnings("Blackjack", m_player.getBalance() - before);
        refreshPlayerStats();
        checkEndCondition();
        m_status.setString("Returned from Blackjack.");
        return;
    }

    if (m_rouletteCard.isClicked(event, window))
    {
        m_audio.playSound("button");
        const double before = m_player.getBalance();
        RouletteUI rouletteUI(m_player, m_audio);
        rouletteUI.run(window);
        m_player.updateGameEarnings("Roulette", m_player.getBalance() - before);
        refreshPlayerStats();
        m_status.setString("Returned from Roulette.");
        return;
    }

    if (m_pokerCard.isClicked(event, window))
    {
        m_audio.playSound("button");
        const double before = m_player.getBalance();
        PokerUI pokerUi(m_player, m_audio);
        pokerUi.run(window);
        m_player.updateGameEarnings("Poker", m_player.getBalance() - before);
        refreshPlayerStats();
        checkEndCondition();
        m_status.setString("Finished Poker.");
        return;
    }

    if (m_slotsCard.isClicked(event, window))
    {
        m_audio.playSound("button");
        const double before = m_player.getBalance();
        SlotsUI slotsUI(m_player, m_audio);
        slotsUI.run(window);
        m_player.updateGameEarnings("Slots", m_player.getBalance() - before);
        refreshPlayerStats();
        checkEndCondition();
        m_status.setString("Returned from Slots.");
        return;
    }

    // ── Bottom buttons ────────────────────────────────────────────────────────
    if (m_saveGameButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        refreshSaveSlotButtons();
        m_showSaveMenu  = true;
        m_pendingReturn = false;
        return;
    }

    if (m_statsButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        buildStatsText();
        m_showStatsPanel = true;
        return;
    }

    if (m_returnButton.isClicked(event, window))
    {
        m_audio.playSound("button");
        if (m_currentSlot > 0)
        {
            m_player.saveToFile(Player::getSlotFilename(m_currentSlot));
            m_status.setString("Auto-saved to Slot " + std::to_string(m_currentSlot) + ".");
            m_running = false;
        }
        else
        {
            refreshSaveSlotButtons();
            m_pendingReturn = true;
            m_showSaveMenu  = true;
        }
        return;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// draw (normal game selection screen)
// ─────────────────────────────────────────────────────────────────────────────
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
    m_statsButton.draw(window);
    m_returnButton.draw(window);

    window.draw(m_status);
}

// ─────────────────────────────────────────────────────────────────────────────
// run
// ─────────────────────────────────────────────────────────────────────────────
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

            if (m_showEndScreen || m_showSaveMenu || m_showStatsPanel)
            {
                handleMouseClick(*event, window);
                continue;
            }

            if (m_currentScreen == Screen::HorseRace)
                m_horseRaceUI->handleEvent(*event);
            else
                handleMouseClick(*event, window);
        }

        // ── Update ───────────────────────────────────────────────────────────
        if (m_showEndScreen)
        {
            m_endMenuButton.update(window);
        }
        else if (m_showSaveMenu)
        {
            for (auto& btn : m_saveSlotButtons) btn.update(window);
            m_saveCancelButton.update(window);
            if (m_pendingReturn) m_saveNoSaveButton.update(window);
        }
        else if (m_showStatsPanel)
        {
            m_statsPanelClose.update(window);
        }
        else if (m_currentScreen == Screen::HorseRace)
        {
            m_horseRaceUI->update(deltaTime);

            if (m_horseRaceUI->wantsToLeave())
            {
                m_horseRaceUI->clearLeaveRequest();
                m_player.updateGameEarnings("Horse Race", m_player.getBalance() - m_horseRaceBalanceBefore);
                m_currentScreen = Screen::Menu;
                m_audio.playMusic("assets/sound/background/main_track.wav", true, 15.f);
                refreshPlayerStats();
                checkEndCondition();
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
            m_statsButton.update(window);
            m_returnButton.update(window);
        }

        // ── Draw ─────────────────────────────────────────────────────────────
        window.clear();

        if (m_showEndScreen)
            drawEndScreen(window);
        else if (m_currentScreen == Screen::HorseRace)
            m_horseRaceUI->draw();
        else
        {
            draw(window);
            if (m_showSaveMenu)
                drawSaveSlotMenu(window);
            else if (m_showStatsPanel)
                drawStatsPanel(window);
        }

        window.display();
    }
}
