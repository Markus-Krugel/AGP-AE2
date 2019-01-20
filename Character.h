#pragma once
#include "Model.h"
#include "GameTimer.h"

class Character : public Model
{
	public:
		Character();
		Character(float health, float damage, float attackRange,float attackCooldown, ID3D11Device* device, ID3D11DeviceContext* context);

		float health, damageOutput, attackRange;

		boolean isDead();
		void takeDamage(float damage);
		void AttackCharacter(Character * character);
		boolean CharacterInAttackRange(Character * character);
		void UpdateTimer();
		void MoveForward(float distance);
		void changeDirection(XMVECTOR target);

	protected:
		float attackCooldown;
		GameTimer* timeSinceLastAttack;
};