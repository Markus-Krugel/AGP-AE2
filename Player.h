#pragma once
#include "Character.h"


class Player : public Character
{
	public:
		Player();
		Player(float health, float damage, float attackRange, float attackCooldown, ID3D11Device* device, ID3D11DeviceContext* context);
};