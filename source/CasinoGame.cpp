// Description: Implementation file for the CasinoGame abstract base class.
// Related Files: CasinoGame.h, Player.h
// Date Created: 3/29/2026
// Last Edited: 4/24/2026

#include "CasinoGame.h"

CasinoGame::CasinoGame()
    : m_rng(std::random_device{}())
{
    // m_rng is seeded here once and inherited by all derived game classes.
}
