#include "Character.h"

Character::Character()
{
}

Character::Character(float health, float damage, float attackRange, float attackCooldown, ID3D11Device * device, ID3D11DeviceContext * context) : Model(device, context)
{
	this->health = health;
	this->damageOutput = damage;
	this->attackRange = attackRange;
	this->attackCooldown = attackCooldown;

	timeSinceLastAttack = new GameTimer();
	timeSinceLastAttack->Reset();
	timeSinceLastAttack->Start();
}

boolean Character::isDead()
{
	if (health <= 0)
		return true;
	else
		return false;
}

void Character::takeDamage(float damage)
{
	health -= damage;
}

void Character::AttackCharacter(Character* character)
{
	float time = timeSinceLastAttack->GameTime();
	if (CharacterInAttackRange(character) && timeSinceLastAttack->GameTime() > attackCooldown)
	{
		character->takeDamage(damageOutput);
		timeSinceLastAttack->Reset();
		timeSinceLastAttack->Start();
	}
}

boolean Character::CharacterInAttackRange(Character* character)
{
	float distanceToCharacter = pow(getPosition().x - character->getPosition().x, 2) + pow(getPosition().z - character->getPosition().z, 2);

	if (distanceToCharacter <= attackRange)
		return true;
	else
		return false;
}

void Character::UpdateTimer()
{
	timeSinceLastAttack->Tick();
}

void Character::MoveForward(float distance)
{
	m_x += directionX * distance;
	m_z += directionZ * distance;
}

void Character::changeDirection(XMVECTOR target)
{
	directionX = target.x - getPosition().x;
	directionZ = target.z - getPosition().z;
}
