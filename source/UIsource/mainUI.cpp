// Description: Implementation file for the MainUI start menu/controller class.
// Related Files: MainUI.h, Player.h, CasinoGameUI.h
// Date Created: 4/7/2026
// Last Edited: 4/24/2026

#include "UIheaders/MainUI.h"
#include "Player.h"
#include "UIheaders/CasinoGameUI.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <optional>
#include <sstream>

// ─────────────────────────────────────────────────────────────────────────────
// Button
// ─────────────────────────────────────────────────────────────────────────────
MainUI::Button::Button(const sf::Font& font,
    const std::string& label,
    sf::Vector2f position,
    sf::Vector2f size)
    : box(size), text(font, label, 36)
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

void MainUI::Button::setLabel(const sf::Font& font, const std::string& label)
{
    text = sf::Text(font, label, 28);
    text.setFillColor(sf::Color::White);
    const sf::FloatRect tb = text.getLocalBounds();
    text.setOrigin({ tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f });
    const sf::FloatRect bb = box.getGlobalBounds();
    text.setPosition({ bb.position.x + bb.size.x / 2.f, bb.position.y + bb.size.y / 2.f });
}

bool MainUI::Button::contains(sf::Vector2f point) const
{
    return box.getGlobalBounds().contains(point);
}

void MainUI::Button::update(const sf::RenderWindow& window)
{
    const sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    const sf::Vector2f mouseCoords =
        window.mapPixelToCoords(mousePixel, window.getDefaultView());

    if (contains(mouseCoords))
        box.setFillColor(sf::Color(90, 90, 130, 240));
    else
        box.setFillColor(sf::Color(40, 40, 60, 220));
}

bool MainUI::Button::isClicked(const sf::Event& event, const sf::RenderWindow& window) const
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

void MainUI::Button::draw(sf::RenderWindow& window) const
{
    window.draw(box);
    window.draw(text);
}

// ─────────────────────────────────────────────────────────────────────────────
// InputField  (still used for the New Game name input)
// ─────────────────────────────────────────────────────────────────────────────
MainUI::InputField::InputField(const sf::Font& font,
    const std::string& placeholderText,
    sf::Vector2f position,
    sf::Vector2f size,
    bool numeric)
    : box(size),
    text(font, "", 30),
    placeholder(placeholderText),
    numericOnly(numeric)
{
    box.setPosition(position);
    box.setFillColor(sf::Color(25, 25, 35, 220));
    box.setOutlineColor(sf::Color(200, 200, 200));
    box.setOutlineThickness(2.f);

    text.setFillColor(sf::Color(210, 210, 210));
    text.setPosition({ position.x + 16.f, position.y + 12.f });

    syncText();
}

bool MainUI::InputField::contains(sf::Vector2f point) const
{
    return box.getGlobalBounds().contains(point);
}

void MainUI::InputField::setActive(bool active)
{
    if (active)
        box.setOutlineColor(sf::Color(255, 215, 0));
    else
        box.setOutlineColor(sf::Color(200, 200, 200));
}

void MainUI::InputField::syncText()
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

void MainUI::InputField::draw(sf::RenderWindow& window) const
{
    window.draw(box);
    window.draw(text);
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────
MainUI::MainUI(AudioManager& audio)
    : m_audio(audio),
    m_font("assets/arial.ttf"),
    m_backgroundTexture("assets/menu_background.jpg"),
    m_backgroundSprite(m_backgroundTexture),
    m_logoTexture("assets/menu_logo.png"),
    m_logoSprite(m_logoTexture),
    m_introTitle(m_font, "CONGO CASH CASINO", 90),
    m_noteSprite(m_noteTexture),
    m_introSubtitle(m_font, "Where Fortune Favors the Bold", 42),
    m_introPrompt(m_font, "Click anywhere or press any key to continue...", 30),
    m_noteContinuePrompt(m_font, "[ Click anywhere to continue ]", 26),
    m_status(m_font, "Main Menu", 32),
    m_statisticsTitle(m_font, "STATISTICS", 58),
    m_statisticsBody(m_font, "", 24),
    m_newGameTitle(m_font, "NEW GAME", 58),
    m_nameLabel(m_font, "Player Name", 34),
    m_loadGameTitle(m_font, "LOAD GAME", 58),
    m_currentScreen(Screen::GrandpaNote),
    m_activeField(ActiveField::None),
    m_newGame(m_font, "New Game", { 760.f, 320.f }, { 400.f, 85.f }),
    m_loadGame(m_font, "Load Game", { 760.f, 430.f }, { 400.f, 85.f }),
    m_statistics(m_font, "Statistics", { 760.f, 540.f }, { 400.f, 85.f }),
    m_instructions(m_font, "Instructions", { 760.f, 650.f }, { 400.f, 85.f }),
    m_exitButton(m_font, "Exit", { 760.f, 760.f }, { 400.f, 85.f }),
    m_backButton(m_font, "Back", { 1500.f, 930.f }, { 260.f, 80.f }),
    m_createPlayerButton(m_font, "Create Player", { 780.f, 760.f }, { 360.f, 85.f }),
    m_nameInput(m_font, "Enter player name", { 660.f, 420.f }, { 600.f, 70.f }, false)
{
    const sf::Vector2u backgroundSize = m_backgroundTexture.getSize();
    m_backgroundSprite.setScale({
        1920.f / static_cast<float>(backgroundSize.x),
        1080.f / static_cast<float>(backgroundSize.y)
        });

    const sf::Vector2u logoSize = m_logoTexture.getSize();
    m_logoSprite.setScale({
        700.f / static_cast<float>(logoSize.x),
        400.f / static_cast<float>(logoSize.y)
        });
    m_logoSprite.setPosition({ 610.f, 25.f });

    // ── Intro screen setup ────────────────────────────────────────────────
    m_introTitle.setFillColor(sf::Color(255, 215, 0, 0));
    {
        const sf::FloatRect b = m_introTitle.getLocalBounds();
        m_introTitle.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f });
        m_introTitle.setPosition({ 960.f, 380.f });
    }

    m_introSubtitle.setFillColor(sf::Color(255, 255, 255, 0));
    {
        const sf::FloatRect b = m_introSubtitle.getLocalBounds();
        m_introSubtitle.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f });
        m_introSubtitle.setPosition({ 960.f, 510.f });
    }

    m_introPrompt.setFillColor(sf::Color(200, 200, 200, 0));
    {
        const sf::FloatRect b = m_introPrompt.getLocalBounds();
        m_introPrompt.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f });
        m_introPrompt.setPosition({ 960.f, 630.f });
    }

    m_introClock.restart();

    // ── Grandpa's note setup ──────────────────────────────────────────────
    m_noteBackground.setSize({ 1920.f, 1080.f });
    m_noteBackground.setFillColor(sf::Color(10, 8, 5));

    m_notePaper.setSize({ 860.f, 680.f });
    m_notePaper.setPosition({ 530.f, 180.f });
    m_notePaper.setFillColor(sf::Color(245, 235, 210));
    m_notePaper.setOutlineColor(sf::Color(160, 130, 80));
    m_notePaper.setOutlineThickness(3.f);

    const std::vector<std::string> noteText = {
        "To whoever this may find,",
        "",
        "If you're reading this, then I'm already gone.",
        "I left you $200 not much, but it's a start.",
        "",
        "Win $5,000 and you'll understand why I left.",
        "Some things can only be learned the hard way.",
        "",
        "Play smart. Trust your gut.",
        "And remember always bet on Hakari.",
        "",
        "Good luck.",
        "",
        "Mistery Man"
    };

    float noteY = 310.f;
    for (const std::string& line : noteText)
    {
        sf::Text t(m_font, line, line.empty() ? 10u : (line.rfind("  —", 0) == 0 ? 30u : 28u));
        t.setFillColor(sf::Color(40, 25, 10));
        t.setPosition({ 690.f, noteY });
        m_noteLines.push_back(t);
        noteY += (line.empty() ? 12.f : 32.f);
    }

    m_noteContinuePrompt.setFillColor(sf::Color(120, 90, 50));
    {
        const sf::FloatRect b = m_noteContinuePrompt.getLocalBounds();
        m_noteContinuePrompt.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y });
        m_noteContinuePrompt.setPosition({ 960.f, 820.f });
    }

    m_noteAlphaClock.restart();

    // ── Main menu setup ───────────────────────────────────────────────────
    m_status.setFillColor(sf::Color::White);
    m_status.setPosition({ 790.f, 950.f });

    m_statisticsTitle.setFillColor(sf::Color::White);
    {
        const sf::FloatRect bounds = m_statisticsTitle.getLocalBounds();
        m_statisticsTitle.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });
        m_statisticsTitle.setPosition({ 960.f, 30.f });
    }

    m_statisticsBody.setFillColor(sf::Color(220, 220, 220));
    m_statisticsBody.setPosition({ 60.f, 700.f });

    m_newGameTitle.setFillColor(sf::Color::White);
    m_newGameTitle.setPosition({ 790.f, 150.f });

    m_nameLabel.setFillColor(sf::Color::White);
    m_nameLabel.setPosition({ 660.f, 360.f });

    m_loadGameTitle.setFillColor(sf::Color::White);
    {
        const sf::FloatRect bounds = m_loadGameTitle.getLocalBounds();
        m_loadGameTitle.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });
        m_loadGameTitle.setPosition({ 960.f, 30.f });
    }

    // ── Instructions setup ────────────────────────────────────────────────
    const std::vector<std::string> lines = {
        "BLACKJACK:",
        "- Goal: get as close to 21 as possible without going over.",
        "- Number cards are worth face value.",
        "- Face cards are worth 10.",
        "- Aces are worth 1 or 11.",
        "- Beat the dealer by having a higher total than the dealer,",
        "  or by the dealer busting.",
        "",
        "ROULETTE:",
        "- Players place bets on where the ball will land.",
        "- Common bets include red/black, odd/even, ranges,",
        "  or exact numbers.",
        "- Matching bets win based on the type of wager.",
        "",
        "SLOTS:",
        "- Spin the reels and try to match symbols.",
        "- Matching combinations pay different amounts.",
        "",
        "POKER:",
        "- Poker compares hand rankings.",
        "- Better hand wins.",
        "- Common hands include pair, two pair, straight,",
        "  flush, and full house.",
        "",
        "HORSE RACES:",
        "- Pick a horse and place a bet before the race starts.",
        "- If your horse wins, you get paid based on the odds.",
        "",
        "GENERAL:",
        "- Manage your balance carefully.",
        "- Save your progress so you can load it later."
    };

    float y = 100.f;
    for (const std::string& line : lines)
    {
        sf::Text textLine(m_font, line, 24);
        textLine.setFillColor(sf::Color::White);

        const sf::FloatRect bounds = textLine.getLocalBounds();
        textLine.setOrigin({
            bounds.position.x + bounds.size.x / 2.f,
            bounds.position.y
            });

        textLine.setPosition({ 960.f, y });

        m_instructionLines.push_back(textLine);
        y += 30.f;
    }

    // ── Save slot buttons (shared by Load Game and Statistics screens) ────
    // Layout: 2 columns of 5, each button 820×80, left col x=55, right col x=1045
    for (int i = 1; i <= 10; ++i)
    {
        const int row = (i <= 5) ? (i - 1) : (i - 6);
        const float x = (i <= 5) ? 55.f : 1045.f;
        const float slotY = 130.f + static_cast<float>(row) * 95.f;
        m_slotButtons.emplace_back(m_font,
            "Slot " + std::to_string(i),
            sf::Vector2f{ x, slotY },
            sf::Vector2f{ 820.f, 80.f });
    }

    refreshSlotButtons();
}

// ─────────────────────────────────────────────────────────────────────────────
// refreshSlotButtons
// ─────────────────────────────────────────────────────────────────────────────
void MainUI::refreshSlotButtons()
{
    for (int i = 1; i <= 10; ++i)
    {
        Player temp("", 0.0);
        if (temp.loadFromFile(Player::getSlotFilename(i)))
        {
            std::ostringstream label;
            label << std::fixed << std::setprecision(2);
            label << "Slot " << i << ":  " << temp.getName()
                  << "   $" << temp.getBalance()
                  << "   [" << temp.getGamesPlayed() << " games]";
            m_slotButtons[static_cast<std::size_t>(i - 1)].setLabel(m_font, label.str());
        }
        else
        {
            m_slotButtons[static_cast<std::size_t>(i - 1)].setLabel(
                m_font, "Slot " + std::to_string(i) + ":  [Empty]");
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Draw: Grandpa's Note
// ─────────────────────────────────────────────────────────────────────────────
void MainUI::drawGrandpaNote(sf::RenderWindow& window)
{
    const float elapsed = m_noteAlphaClock.getElapsedTime().asSeconds();
    m_noteAlpha = std::min(elapsed / 1.5f, 1.0f) * 255.f;
    const uint8_t a = static_cast<uint8_t>(m_noteAlpha);

    uint8_t promptAlpha = 0;
    if (elapsed > 1.5f)
    {
        const float blink = std::sin((elapsed - 1.5f) * 2.5f);
        promptAlpha = static_cast<uint8_t>(std::max(0.f, blink) * 200.f);
    }

    m_noteBackground.setFillColor(sf::Color(10, 8, 5, a));
    window.draw(m_noteBackground);

    sf::Color paperColor = m_notePaper.getFillColor();
    paperColor.a = a;
    m_notePaper.setFillColor(paperColor);

    sf::Color outlineColor = m_notePaper.getOutlineColor();
    outlineColor.a = a;
    m_notePaper.setOutlineColor(outlineColor);
    window.draw(m_notePaper);

    for (sf::Text line : m_noteLines)
    {
        sf::Color c = line.getFillColor();
        c.a = a;
        line.setFillColor(c);
        window.draw(line);
    }

    m_noteContinuePrompt.setFillColor(sf::Color(120, 90, 50, promptAlpha));
    window.draw(m_noteContinuePrompt);
}

// ─────────────────────────────────────────────────────────────────────────────
// Draw: Intro
// ─────────────────────────────────────────────────────────────────────────────
void MainUI::drawIntroScreen(sf::RenderWindow& window)
{
    window.draw(m_backgroundSprite);

    const float elapsed = m_introClock.getElapsedTime().asSeconds();
    m_introAlpha = std::min(elapsed / 2.0f, 1.0f) * 255.f;
    const uint8_t a = static_cast<uint8_t>(m_introAlpha);

    uint8_t promptAlpha = a;
    if (elapsed > 2.5f)
    {
        const float blink = std::sin((elapsed - 2.5f) * 3.14f);
        promptAlpha = static_cast<uint8_t>(std::max(0.f, blink) * 255.f);
    }

    m_introTitle.setFillColor(sf::Color(255, 215, 0, a));
    m_introSubtitle.setFillColor(sf::Color(255, 255, 255, a));
    m_introPrompt.setFillColor(sf::Color(200, 200, 200, promptAlpha));

    window.draw(m_introTitle);
    window.draw(m_introSubtitle);
    window.draw(m_introPrompt);
}

// ─────────────────────────────────────────────────────────────────────────────
// Draw: Main Menu
// ─────────────────────────────────────────────────────────────────────────────
void MainUI::drawMainMenu(sf::RenderWindow& window)
{
    window.draw(m_backgroundSprite);
    window.draw(m_logoSprite);
    m_newGame.draw(window);
    m_loadGame.draw(window);
    m_statistics.draw(window);
    m_instructions.draw(window);
    m_exitButton.draw(window);
    window.draw(m_status);
}

// ─────────────────────────────────────────────────────────────────────────────
// Draw: Instructions
// ─────────────────────────────────────────────────────────────────────────────
void MainUI::drawInstructionsScreen(sf::RenderWindow& window)
{
    window.draw(m_backgroundSprite);

    for (const sf::Text& line : m_instructionLines)
        window.draw(line);

    m_backButton.draw(window);
}

// ─────────────────────────────────────────────────────────────────────────────
// Draw: Statistics
// ─────────────────────────────────────────────────────────────────────────────
void MainUI::drawStatisticsScreen(sf::RenderWindow& window)
{
    window.draw(m_backgroundSprite);

    if (!m_statsSlotSelected)
    {
        // ── Slot selection view ───────────────────────────────────────────
        window.draw(m_statisticsTitle);

        sf::Text hint(m_font, "Select a save slot to view its statistics:", 28);
        hint.setFillColor(sf::Color(200, 200, 200));
        {
            const sf::FloatRect b = hint.getLocalBounds();
            hint.setOrigin({ b.position.x + b.size.x / 2.f, b.position.y });
            hint.setPosition({ 960.f, 105.f });
        }
        window.draw(hint);

        for (Button& btn : m_slotButtons)
            btn.draw(window);

        m_backButton.draw(window);
    }
    else
    {
        // ── Full-screen stats panel ───────────────────────────────────────
        sf::RectangleShape overlay({ 1920.f, 1080.f });
        overlay.setFillColor(sf::Color(5, 5, 15, 210));
        window.draw(overlay);

        sf::RectangleShape panel({ 1400.f, 870.f });
        panel.setPosition({ 260.f, 100.f });
        panel.setFillColor(sf::Color(18, 18, 35, 240));
        panel.setOutlineColor(sf::Color(255, 215, 0));
        panel.setOutlineThickness(2.f);
        window.draw(panel);

        // Gold divider under title area
        sf::RectangleShape divider({ 1360.f, 2.f });
        divider.setPosition({ 280.f, 195.f });
        divider.setFillColor(sf::Color(255, 215, 0, 160));
        window.draw(divider);

        // Second divider above per-game section
        sf::RectangleShape divider2({ 1360.f, 2.f });
        divider2.setPosition({ 280.f, 385.f });
        divider2.setFillColor(sf::Color(255, 215, 0, 80));
        window.draw(divider2);

        m_statisticsBody.setPosition({ 290.f, 120.f });
        window.draw(m_statisticsBody);

        sf::Text backHint(m_font, "[ Back ] - Return to slot selection", 24);
        backHint.setFillColor(sf::Color(160, 160, 160));
        backHint.setPosition({ 290.f, 995.f });
        window.draw(backHint);

        m_backButton.draw(window);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Draw: New Game
// ─────────────────────────────────────────────────────────────────────────────
void MainUI::drawNewGameScreen(sf::RenderWindow& window)
{
    window.draw(m_backgroundSprite);
    window.draw(m_newGameTitle);
    window.draw(m_nameLabel);
    m_nameInput.draw(window);
    m_createPlayerButton.draw(window);
    m_backButton.draw(window);
    window.draw(m_status);
}

// ─────────────────────────────────────────────────────────────────────────────
// Draw: Load Game  (10 slot buttons)
// ─────────────────────────────────────────────────────────────────────────────
void MainUI::drawLoadGameScreen(sf::RenderWindow& window)
{
    window.draw(m_backgroundSprite);
    window.draw(m_loadGameTitle);

    for (Button& btn : m_slotButtons)
        btn.draw(window);

    m_backButton.draw(window);
    window.draw(m_status);
}

// ─────────────────────────────────────────────────────────────────────────────
// Input helpers
// ─────────────────────────────────────────────────────────────────────────────
void MainUI::handleBackspace()
{
    if (m_activeField == ActiveField::NewGameName)
    {
        if (!m_nameInput.value.empty())
            m_nameInput.value.pop_back();
        m_nameInput.syncText();
    }
}

void MainUI::handleTextEntered(const sf::Event& event)
{
    if (m_currentScreen == Screen::GrandpaNote)
    {
        m_currentScreen = Screen::Intro;
        m_introClock.restart();
        return;
    }

    if (m_currentScreen == Screen::Intro)
    {
        m_currentScreen = Screen::MainMenu;
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

    if (m_activeField == ActiveField::NewGameName && m_nameInput.value.size() < 24)
    {
        m_nameInput.value.push_back(character);
        m_nameInput.syncText();
    }
}

void MainUI::clearFormState()
{
    m_activeField = ActiveField::None;
    m_nameInput.setActive(false);
}

// ─────────────────────────────────────────────────────────────────────────────
// startNewGame  — slot 0 means the CasinoGameUI will prompt to pick a slot
// ─────────────────────────────────────────────────────────────────────────────
void MainUI::startNewGame(sf::RenderWindow& window)
{
    if (m_nameInput.value.empty())
    {
        m_status.setString("Name cannot be empty.");
        return;
    }

    Player player(m_nameInput.value, 200.0);

    {
        std::ostringstream stream;
        stream << "Created player: " << player.getName()
            << " | Balance: $" << player.getBalance();
        m_status.setString(stream.str());
    }

    CasinoGameUI casinoMenu(player, m_audio, 0);
    casinoMenu.run(window);

    m_currentScreen = Screen::MainMenu;
    clearFormState();
    refreshSlotButtons();
}

// ─────────────────────────────────────────────────────────────────────────────
// handleMouseClick
// ─────────────────────────────────────────────────────────────────────────────
void MainUI::handleMouseClick(const sf::Event& event, sf::RenderWindow& window)
{
    if (m_currentScreen == Screen::GrandpaNote)
    {
        if (event.getIf<sf::Event::MouseButtonPressed>())
        {
            m_currentScreen = Screen::Intro;
            m_introClock.restart();
        }
        return;
    }

    if (m_currentScreen == Screen::Intro)
    {
        if (event.getIf<sf::Event::MouseButtonPressed>())
            m_currentScreen = Screen::MainMenu;
        return;
    }

    if (m_currentScreen == Screen::MainMenu)
    {
        if (m_newGame.isClicked(event, window))
        {
            m_audio.playSound("button");
            m_currentScreen = Screen::NewGame;
            m_status.setString("Create a new player.");
            clearFormState();
            return;
        }

        if (m_loadGame.isClicked(event, window))
        {
            m_audio.playSound("button");
            m_currentScreen = Screen::LoadGame;
            m_status.setString("Select a slot to load.");
            refreshSlotButtons();
            clearFormState();
            return;
        }

        if (m_statistics.isClicked(event, window))
        {
            m_audio.playSound("button");
            m_currentScreen = Screen::Statistics;
            m_statisticsBody.setString("");
            m_statsSlotSelected = false;
            refreshSlotButtons();
            clearFormState();
            return;
        }

        if (m_instructions.isClicked(event, window))
        {
            m_audio.playSound("button");
            m_currentScreen = Screen::Instructions;
            clearFormState();
            return;
        }

        if (m_exitButton.isClicked(event, window))
        {
            m_audio.playSound("button");
            window.close();
            return;
        }
    }
    else if (m_currentScreen == Screen::Instructions)
    {
        if (m_backButton.isClicked(event, window))
        {
            m_audio.playSound("button");
            m_currentScreen = Screen::MainMenu;
            m_status.setString("Main Menu");
            return;
        }
    }
    else if (m_currentScreen == Screen::Statistics)
    {
        if (m_backButton.isClicked(event, window))
        {
            m_audio.playSound("button");
            if (m_statsSlotSelected)
            {
                m_statsSlotSelected = false;
                m_statisticsBody.setString("");
            }
            else
            {
                m_currentScreen = Screen::MainMenu;
                m_statisticsBody.setString("");
            }
            return;
        }

        if (!m_statsSlotSelected)
        {
            const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>();
            if (mousePressed && mousePressed->button == sf::Mouse::Button::Left)
            {
                const sf::Vector2f mouseCoords =
                    window.mapPixelToCoords(mousePressed->position, window.getDefaultView());

                for (int i = 0; i < 10; ++i)
                {
                    if (m_slotButtons[static_cast<std::size_t>(i)].contains(mouseCoords))
                    {
                        m_audio.playSound("button");
                        Player temp("", 0.0);
                        if (!temp.loadFromFile(Player::getSlotFilename(i + 1)))
                        {
                            m_status.setString("Slot " + std::to_string(i + 1) + " is empty.");
                        }
                        else
                        {
                            const int played  = temp.getGamesPlayed();
                            const int won     = temp.getGamesWon();
                            const int lost    = temp.getGamesLost();
                            const double winPct = played > 0
                                ? 100.0 * won / played : 0.0;
                            const double net  = temp.getNetProfit();

                            std::ostringstream ss;
                            ss << std::fixed << std::setprecision(2);

                            // Header line — player name in the body (drawn as one sf::Text)
                            ss << temp.getName() << "   (Slot " << (i + 1) << ")\n\n";

                            // Overall financials
                            ss << "Balance:         $" << temp.getBalance() << "\n";
                            ss << "Total Wagered:   $" << temp.getTotalBetted() << "\n";
                            ss << "Total Returned:  $" << temp.getTotalWon() << "\n";
                            ss << "Net Profit:      " << (net >= 0.0 ? "+" : "") << "$" << net << "\n\n";

                            // Overall W/L
                            ss << "Games Played:  " << played
                               << "   Won: " << won
                               << "   Lost: " << lost;
                            if (played > 0)
                                ss << "   ("
                                   << std::setprecision(1) << winPct << "% win rate)";
                            ss << "\n\n";
                            ss << std::setprecision(2);

                            // Per-game breakdown
                            ss << "Per-Game Breakdown:\n";
                            const auto& stats = temp.getGameStats();
                            if (stats.empty())
                            {
                                ss << "  No game history recorded yet.\n";
                            }
                            else
                            {
                                for (const auto& [name, stat] : stats)
                                {
                                    const double gNet = stat.netEarnings;
                                    ss << "  " << name << ":   "
                                       << stat.played << " played,  "
                                       << stat.won << " won,  "
                                       << stat.played - stat.won << " lost   |   Net: "
                                       << (gNet >= 0.0 ? "+" : "") << "$" << gNet << "\n";
                                }
                            }

                            ss << "\n";
                            ss << "Highest Earning Game:  " << temp.getHighestEarningGame() << "\n";
                            ss << "Most Wins Game:        " << temp.getMostWinsGame();

                            m_statisticsBody.setCharacterSize(26);
                            m_statisticsBody.setString(ss.str());
                            m_statsSlotSelected = true;
                        }
                        return;
                    }
                }
            }
        }
    }
    else if (m_currentScreen == Screen::NewGame)
    {
        const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>();
        if (mousePressed && mousePressed->button == sf::Mouse::Button::Left)
        {
            const sf::Vector2f mouseCoords =
                window.mapPixelToCoords(mousePressed->position, window.getDefaultView());

            const bool clickedName = m_nameInput.contains(mouseCoords);
            m_nameInput.setActive(clickedName);
            m_activeField = clickedName ? ActiveField::NewGameName : ActiveField::None;
        }

        if (m_createPlayerButton.isClicked(event, window))
        {
            m_audio.playSound("button");
            startNewGame(window);
            return;
        }

        if (m_backButton.isClicked(event, window))
        {
            m_audio.playSound("button");
            m_currentScreen = Screen::MainMenu;
            m_status.setString("Main Menu");
            clearFormState();
            return;
        }
    }
    else if (m_currentScreen == Screen::LoadGame)
    {
        if (m_backButton.isClicked(event, window))
        {
            m_audio.playSound("button");
            m_currentScreen = Screen::MainMenu;
            m_status.setString("Main Menu");
            return;
        }

        const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>();
        if (mousePressed && mousePressed->button == sf::Mouse::Button::Left)
        {
            const sf::Vector2f mouseCoords =
                window.mapPixelToCoords(mousePressed->position, window.getDefaultView());

            for (int i = 0; i < 10; ++i)
            {
                if (m_slotButtons[static_cast<std::size_t>(i)].contains(mouseCoords))
                {
                    m_audio.playSound("button");
                    Player player("", 0.0);
                    if (!player.loadFromFile(Player::getSlotFilename(i + 1)))
                    {
                        m_status.setString("Slot " + std::to_string(i + 1) + " is empty.");
                    }
                    else
                    {
                        CasinoGameUI casinoMenu(player, m_audio, i + 1);
                        casinoMenu.run(window);

                        m_currentScreen = Screen::MainMenu;
                        m_status.setString("Main Menu");
                        refreshSlotButtons();
                    }
                    return;
                }
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Main loop
// ─────────────────────────────────────────────────────────────────────────────
void MainUI::run(sf::RenderWindow& window)
{
    while (window.isOpen())
    {
        m_audio.update();
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::TextEntered>())
                handleTextEntered(*event);

            handleMouseClick(*event, window);
        }

        // Update hover states
        if (m_currentScreen == Screen::MainMenu)
        {
            m_newGame.update(window);
            m_loadGame.update(window);
            m_statistics.update(window);
            m_instructions.update(window);
            m_exitButton.update(window);
        }
        else if (m_currentScreen == Screen::Instructions)
        {
            m_backButton.update(window);
        }
        else if (m_currentScreen == Screen::Statistics)
        {
            m_backButton.update(window);
            if (!m_statsSlotSelected)
            {
                for (Button& btn : m_slotButtons)
                    btn.update(window);
            }
        }
        else if (m_currentScreen == Screen::NewGame)
        {
            m_createPlayerButton.update(window);
            m_backButton.update(window);
        }
        else if (m_currentScreen == Screen::LoadGame)
        {
            m_backButton.update(window);
            for (Button& btn : m_slotButtons)
                btn.update(window);
        }

        window.clear();

        switch (m_currentScreen)
        {
        case Screen::GrandpaNote:
            drawGrandpaNote(window);
            break;
        case Screen::Intro:
            drawIntroScreen(window);
            break;
        case Screen::MainMenu:
            drawMainMenu(window);
            break;
        case Screen::Instructions:
            drawInstructionsScreen(window);
            break;
        case Screen::Statistics:
            drawStatisticsScreen(window);
            break;
        case Screen::NewGame:
            drawNewGameScreen(window);
            break;
        case Screen::LoadGame:
            drawLoadGameScreen(window);
            break;
        }

        window.display();
    }
}
