#pragma once

#include "Gameplay/Gameplay.h"
#include "Gameplay/GameSystem.h"

class Player;

class PlayerSystem : public GameSystem
{
public:
	MACRO_DeclareTypeInfo(PlayerSystem);

	void RegisterPlayer(WeakRef<Player> player) { m_Player = player; }
	Player* GetPlayer() { return m_Player.Get(); }
	const Player* GetPlayer() const { return m_Player.Get(); }

private:
	WeakRef<Player> m_Player;
};
