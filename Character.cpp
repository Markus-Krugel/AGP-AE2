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

// checks if the character is alive
boolean Character::isDead()
{
	if (health <= 0)
		return true;
	else
		return false;
}

// the character will lose damage
void Character::takeDamage(float damage)
{
	health -= damage;
}

// attacks the other character when the attack cooldown is over and the other character in attack range
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

// checks if the other character is in attack range
boolean Character::CharacterInAttackRange(Character* character)
{
	float distanceToCharacter = pow(getPosition().x - character->getPosition().x, 2) + pow(getPosition().z - character->getPosition().z, 2);

	if (distanceToCharacter <= attackRange)
		return true;
	else
		return false;
}

// updates the cooldown timer
void Character::UpdateTimer()
{
	timeSinceLastAttack->Tick();
}

// moves the character forward
void Character::MoveForward(float distance)
{
	m_x += directionX * distance;
	m_z += directionZ * distance;
}

// changes the forward direction
void Character::changeDirection(XMVECTOR target)
{
	directionX = target.x - getPosition().x;
	directionZ = target.z - getPosition().z;
}
