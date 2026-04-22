// Header file for Roulette UI/controller class.
// Related Files: routtleui.cpp, Roulette.h, Player.h, Audio.h
// Date Created: 4/21/2026
// Last Edited: 4/21/2026

#pragma once

#include <SFML/Graphics.hpp>

#include <string>
#include <type_traits>

#include "Player.h"
#include "Roulette.h"

class AudioManager;

class RouletteUI
{
public:
    RouletteUI(Player& player, AudioManager& audio);
    void run(sf::RenderWindow& window);

private:
    enum class ActiveField
    {
        None,
        BetAmount,
        Number
    };

    struct Button
    {
        sf::RectangleShape box;
        sf::Text text;
        bool selected;
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
        bool numericOnly;
        bool decimalAllowed;

        InputField(const sf::Font& font,
            const std::string& placeholderText,
            sf::Vector2f position,
            sf::Vector2f size,
            bool numeric = false,
            bool allowDecimal = false);

        bool contains(sf::Vector2f point) const;
        void setActive(bool active);
        void syncText();
        void draw(sf::RenderWindow& window) const;
    };

    void refreshPlayerStats();
    void refreshSelectionState();
    void refreshStatus(const std::string& text);
    void refreshLastSpinTexts();
    void setBetType(Roulette::BetType type);
    void clearRoundInputs();

    void startSpinAnimation(const Roulette::RoundOutcome& outcome);
    void completeSpinAnimation();
    void updateSpinAnimation();

    void handleBackspace();
    void handleTextEntered(const sf::Event& event);
    void handleMouseClick(const sf::Event& event, sf::RenderWindow& window);

    void draw(sf::RenderWindow& window);

    bool tryParseBetAmount(double& amount) const;
    bool tryParseBetNumber(int& number) const;
    Roulette::BetRequest buildBetFromInputs(bool& valid) const;

    template <typename NumericType>
    bool tryParseRangedValue(const std::string& text,
        NumericType& value,
        NumericType minValue,
        NumericType maxValue) const
    {
        static_assert(std::is_arithmetic_v<NumericType>,
            "RouletteUI::tryParseRangedValue requires a numeric type.");

        if (text.empty())
            return false;

        std::size_t processed = 0;

        try
        {
            if constexpr (std::is_integral_v<NumericType>)
                value = static_cast<NumericType>(std::stoll(text, &processed));
            else
                value = static_cast<NumericType>(std::stod(text, &processed));
        }
        catch (...)
        {
            return false;
        }

        return processed == text.size()
            && value >= minValue
            && value <= maxValue;
    }

    Player& m_player;
    AudioManager& m_audio;
    Roulette m_roulette;

    bool m_running;
    bool m_spinning;

    ActiveField m_activeField;
    Roulette::BetType m_selectedBetType;

    Roulette::RoundOutcome m_lastOutcome;
    Roulette::RoundOutcome m_pendingOutcome;
    bool m_hasLastOutcome;
    bool m_hasPendingOutcome;

    float m_wheelAngle;
    float m_spinStartAngle;
    float m_spinTargetAngle;
    float m_spinDurationSeconds;
    sf::Clock m_spinClock;

    sf::Font m_font;

    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

    sf::Texture m_tableTexture;
    sf::Sprite m_tableSprite;

    sf::Texture m_wheelTexture;
    sf::Sprite m_wheelSprite;

    sf::RectangleShape m_tableFallback;
    sf::CircleShape m_wheelFallback;

    sf::RectangleShape m_topBar;
    sf::RectangleShape m_bottomPanel;
    sf::RectangleShape m_sidePanel;
    sf::RectangleShape m_resultPanel;

    sf::CircleShape m_resultBadge;
    sf::ConvexShape m_wheelMarker;

    sf::Text m_title;
    sf::Text m_subtitle;
    sf::Text m_status;
    sf::Text m_playerNameText;
    sf::Text m_balanceText;
    sf::Text m_selectionTitle;
    sf::Text m_betAmountLabel;
    sf::Text m_numberLabel;
    sf::Text m_lastSpinTitle;
    sf::Text m_lastSpinValue;
    sf::Text m_lastOutcomeText;
    sf::Text m_helpText;

    Button m_numberButton;
    Button m_redButton;
    Button m_blackButton;
    Button m_evenButton;
    Button m_oddButton;
    Button m_spinButton;
    Button m_clearButton;
    Button m_returnButton;

    InputField m_betAmountInput;
    InputField m_numberInput;
};
