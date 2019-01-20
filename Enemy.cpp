#include "Enemy.h"

Enemy::Enemy()
{
	//stateMachine = new StateMachine(*this);
}

Enemy::Enemy(float sightRange, float health, float damage, float attackRange, float attackCooldown, ID3D11Device * device, ID3D11DeviceContext * context, Player* player) : Character(health, damage, attackRange, attackCooldown, device, context)
{
	this->player = player;
	this->sightRange = sightRange;
	this->health = health;
	this->damageOutput = damage;
	this->attackRange = attackRange;

	//stateMachine = new StateMachine(*this);
}

void Enemy::ChangeState(State newState)
{
	currentState = newState;

	float distanceToPatrolPosition1 = pow(getPosition().x - patrolPosition1.x, 2) + pow(getPosition().z - patrolPosition1.z, 2);
	float distanceToPatrolPosition2 = pow(getPosition().x - patrolPosition2.x, 2) + pow(getPosition().z - patrolPosition2.z, 2);


	switch (currentState)
	{	
		case Patrol:
			if (distanceToPatrolPosition1 <= distanceToPatrolPosition2)
			{
				changeDirection(patrolPosition1);
				currentTarget = patrolPosition1;
			}
			else
			{
				changeDirection(patrolPosition2);
				currentTarget = patrolPosition2;
			}
			break;
		case Chase:
			changeDirection(player->getPosition());
			break;
	}
}

void Enemy::setPatrolPositions(XMVECTOR position1, XMVECTOR position2)
{
	patrolPosition1 = position1;
	patrolPosition2 = position2;
}

void Enemy::Update()
{
	float distanceToPatrolPosition1 = pow(getPosition().x - patrolPosition1.x, 2) + pow(getPosition().z - patrolPosition1.z, 2);
	float distanceToPatrolPosition2 = pow(getPosition().x - patrolPosition2.x, 2) + pow(getPosition().z - patrolPosition2.z, 2);

	switch (currentState)
	{
		case Patrol:
			if (!PlayerInSight())
			{
				if (XMVector3Equal(currentTarget, patrolPosition1))
				{
					if (distanceToPatrolPosition1 <= 0.5f)
					{
						currentTarget = patrolPosition2;
						changeDirection(patrolPosition2);
					}
				}

				else if (XMVector3Equal(currentTarget, patrolPosition2))
				{
					if (distanceToPatrolPosition2 <= 0.5f)
					{
						currentTarget = patrolPosition1;
						changeDirection(patrolPosition1);
					}
				}
				MoveForward(0.00025f);
			}
			else
			{
				ChangeState(Chase);
			}
			break;
		case Chase:
			if (PlayerInSight())
			{
				AttackCharacter(player);
				
				if (!CharacterInAttackRange(player))
				{
					changeDirection(player->getPosition());
					MoveForward(0.00025f);
				}
			}
			else
			{
				ChangeState(Patrol);
			}
			break;
	}
}

boolean Enemy::PlayerInSight()
{
	float distanceToPlayer = pow(getPosition().x - player->getPosition().x, 2) + pow(getPosition().z - player->getPosition().z, 2);

	if (distanceToPlayer <= sightRange)
		return true;
	else
		return false;
}

