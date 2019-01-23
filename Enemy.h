#pragma once
#include "Character.h"
#include "Player.h"
//#include "StateMachine.h"

enum State
{
	Patrol,
	Chase
};

class Enemy : public Character
{
	public:
		Enemy();
		Enemy(float sightRange, float health, float damage, float attackRange, float attackCooldown, ID3D11Device* device, ID3D11DeviceContext* context, Player* player);
		Player* player;
		XMVECTOR patrolPosition1;
		XMVECTOR patrolPosition2;
		XMVECTOR currentTarget;
		void ChangeState(State state);
		void setPatrolPositions(XMVECTOR position1, XMVECTOR position2);
		void Update(float time);		
		boolean PlayerInSight();

	protected:
		float sightRange;
		State currentState;
		//StateMachine* stateMachine;
};