// Blackjack minigame implementation file.
// Related Files: Blackjack.h, CardGame.h, BlackjackUI.h
// Date Created: 3/29/2026
// Last Edited: 4/24/2026

#include "Blackjack.h"

Blackjack::Blackjack()
    : m_currentBet(0.0),
    m_roundState(RoundState::WaitingForBet),
    m_roundResult(RoundResult::None),
    m_statusText("Enter your bet and press Deal."),
    m_hideDealerHoleCard(true)
{
    resetDeck(CardGame::DeckStyle::Long);
}

void Blackjack::resetRound()
{
    resetDeck(CardGame::DeckStyle::Long);
    m_playerHand.clearHand();
    m_dealerHand.clearHand();

    m_currentBet = 0.0;
    m_roundState = RoundState::WaitingForBet;
    m_roundResult = RoundResult::None;
    m_statusText = "Enter your bet and press Deal.";
    m_hideDealerHoleCard = true;
}

bool Blackjack::startRound(Player& player, double bet)
{
    if (m_roundState == RoundState::PlayerTurn || m_roundState == RoundState::DealerTurn)
    {
        m_statusText = "Finish the current round first.";
        return false;
    }

    if (bet <= 0.0)
    {
        m_statusText = "Bet must be greater than $0.00.";
        return false;
    }

    if (!player.placeBet(bet))
    {
        m_statusText = "Invalid bet or not enough balance.";
        return false;
    }

    resetDeck(CardGame::DeckStyle::Long);
    m_playerHand.clearHand();
    m_dealerHand.clearHand();

    m_currentBet = bet;
    m_roundState = RoundState::PlayerTurn;
    m_roundResult = RoundResult::None;
    m_hideDealerHoleCard = true;
    m_statusText = "Your turn. Hit or Stand.";

    m_playerHand.hit(*this);
    m_dealerHand.hit(*this);
    m_playerHand.hit(*this);
    m_dealerHand.hit(*this);

    const int playerValue = m_playerHand.getHandValue();
    const int dealerValue = m_dealerHand.getHandValue();

    if (playerValue == 21 && dealerValue == 21)
    {
        m_roundResult = RoundResult::Push;
        settleRound(player);
    }
    else if (playerValue == 21)
    {
        m_roundResult = RoundResult::PlayerBlackjack;
        settleRound(player);
    }
    else if (dealerValue == 21)
    {
        m_roundResult = RoundResult::DealerBlackjack;
        settleRound(player);
    }

    return true;
}

void Blackjack::playerHit(Player& player)
{
    if (m_roundState != RoundState::PlayerTurn)
        return;

    m_playerHand.hit(*this);

    const int playerValue = m_playerHand.getHandValue();

    if (playerValue > 21)
    {
        m_roundResult = RoundResult::PlayerBust;
        settleRound(player);
        return;
    }

    if (playerValue == 21)
    {
        playerStand(player);
        return;
    }

    m_statusText = "Your turn. Hit or Stand.";
}

void Blackjack::playerStand(Player& player)
{
    if (m_roundState != RoundState::PlayerTurn)
        return;

    m_roundState = RoundState::DealerTurn;
    m_hideDealerHoleCard = false;

    m_dealerHand.takeTurn(*this);

    const int playerValue = m_playerHand.getHandValue();
    const int dealerValue = m_dealerHand.getHandValue();

    if (dealerValue > 21)
        m_roundResult = RoundResult::DealerBust;
    else if (playerValue > dealerValue)
        m_roundResult = RoundResult::PlayerWin;
    else if (dealerValue > playerValue)
        m_roundResult = RoundResult::DealerWin;
    else
        m_roundResult = RoundResult::Push;

    settleRound(player);
}

Blackjack::RoundState Blackjack::getRoundState() const
{
    return m_roundState;
}

Blackjack::RoundResult Blackjack::getRoundResult() const
{
    return m_roundResult;
}

std::vector<Blackjack::CardView> Blackjack::getPlayerCards() const
{
    return toCardViews(m_playerHand.getHand());
}

std::vector<Blackjack::CardView> Blackjack::getDealerCards() const
{
    return toCardViews(m_dealerHand.getHand());
}

int Blackjack::getPlayerValue() const
{
    return m_playerHand.getHandValue();
}

int Blackjack::getDealerValue() const
{
    return m_dealerHand.getHandValue();
}

int Blackjack::getDealerVisibleValue() const
{
    if (!m_hideDealerHoleCard)
        return m_dealerHand.getHandValue();

    const std::vector<CardGame::Card>& cards = m_dealerHand.getHand().getCards();

    if (cards.size() <= 1)
        return 0;

    int total = 0;
    int aces = 0;

    for (std::size_t i = 1; i < cards.size(); ++i)
    {
        total += cards[i].value;
        if (cards[i].value == 11)
            ++aces;
    }

    while (total > 21 && aces > 0)
    {
        total -= 10;
        --aces;
    }

    return total;
}

double Blackjack::getCurrentBet() const
{
    return m_currentBet;
}

std::string Blackjack::getStatusText() const
{
    return m_statusText;
}

bool Blackjack::isRoundOver() const
{
    return m_roundState == RoundState::RoundOver;
}

bool Blackjack::isDealerHoleCardHidden() const
{
    return m_hideDealerHoleCard;
}

bool Blackjack::isPlayerTurn() const
{
    return m_roundState == RoundState::PlayerTurn;
}

bool Blackjack::didPlayerWin() const
{
    return m_roundResult == RoundResult::PlayerBlackjack ||
           m_roundResult == RoundResult::DealerBust      ||
           m_roundResult == RoundResult::PlayerWin;
}

// ── Hand ─────────────────────────────────────────────────────────────────────

void Blackjack::Hand::addCard(const CardGame::Card& card)
{
    m_cards.push_back(card);
}

int Blackjack::Hand::getValue() const
{
    int total = 0;
    int aces  = 0;

    for (const CardGame::Card& card : m_cards)
    {
        total += card.value;
        if (card.value == 11)
            ++aces;
    }

    while (total > 21 && aces > 0)
    {
        total -= 10;
        --aces;
    }

    return total;
}

void Blackjack::Hand::clear()
{
    m_cards.clear();
}

const std::vector<CardGame::Card>& Blackjack::Hand::getCards() const
{
    return m_cards;
}

// ── Participant ───────────────────────────────────────────────────────────────

void Blackjack::Participant::hit(Blackjack& game)
{
    m_hand.addCard(game.drawCard());
}

int Blackjack::Participant::getHandValue() const
{
    return m_hand.getValue();
}

const Blackjack::Hand& Blackjack::Participant::getHand() const
{
    return m_hand;
}

void Blackjack::Participant::clearHand()
{
    m_hand.clear();
}

void Blackjack::BJPlayer::takeTurn(Blackjack&)
{
    // Player turn is handled externally via playerHit/playerStand.
}

void Blackjack::Dealer::takeTurn(Blackjack& game)
{
    while (getHandValue() < 17)
    {
        hit(game);
    }
}

// ── Results ───────────────────────────────────────────────────────────────────

void Blackjack::settleRound(Player& player)
{
    m_roundState = RoundState::RoundOver;
    m_hideDealerHoleCard = false;

    switch (m_roundResult)
    {
    case RoundResult::PlayerBlackjack:
        player.addWinnings(m_currentBet * 2.5);
        m_statusText = "Blackjack! You win.";
        break;

    case RoundResult::DealerBlackjack:
        m_statusText = "Dealer has Blackjack. You lose.";
        break;

    case RoundResult::PlayerBust:
        m_statusText = "Bust! You lose.";
        break;

    case RoundResult::DealerBust:
        player.addWinnings(m_currentBet * 2.0);
        m_statusText = "Dealer busts! You win.";
        break;

    case RoundResult::PlayerWin:
        player.addWinnings(m_currentBet * 2.0);
        m_statusText = "You win!";
        break;

    case RoundResult::DealerWin:
        m_statusText = "Dealer wins.";
        break;

    case RoundResult::Push:
        player.addWinnings(m_currentBet);
        m_statusText = "Push. Your bet was returned.";
        break;

    case RoundResult::None:
    default:
        break;
    }
}

std::vector<Blackjack::CardView> Blackjack::toCardViews(const Hand& hand) const
{
    std::vector<CardView> cards;
    cards.reserve(hand.getCards().size());

    for (const CardGame::Card& card : hand.getCards())
    {
        cards.push_back({ card.rank, card.suit, card.value });
    }

    return cards;
}
