#include "Player.h"

Player::Player()
{
}

Player::Player(float health, float damage, float attackRange, float attackCooldown, ID3D11Device * device, ID3D11DeviceContext * context) : Character(health, damage, attackRange, attackCooldown, device, context)
{
	this->health = health;
	this->damageOutput = damage;
	this->attackRange = attackRange;
	this->m_pD3DDevice = device;
	this->m_pImmediateContext = context;
}

