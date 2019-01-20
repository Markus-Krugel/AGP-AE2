//#include "State.h"
//
//// The functions for the patrol state
//void Patrol::EntryAction(Enemy* owner)
//{
//	float distanceToPatrolPosition1 = pow(owner->getPosition().x + owner->patrolPosition1.x, 2) + pow(owner->getPosition().z + owner->patrolPosition1.z, 2);
//	float distanceToPatrolPosition2 = pow(owner->getPosition().x + owner->patrolPosition2.x, 2) + pow(owner->getPosition().z + owner->patrolPosition2.z, 2);
//
//	if (distanceToPatrolPosition1 <= distanceToPatrolPosition2)
//		owner->MoveTowards(owner->patrolPosition1);
//	else
//		owner->MoveTowards(owner->patrolPosition2);
//}
//
//void Patrol::Execute(Enemy* owner)
//{
//	if (!owner->PlayerInSight())
//		owner->MoveForward(0.00075f);
//	else
//	{
//		Chase* chase = new Chase();
//		owner->ChangeState(chase);
//	}
//}
//
//void Patrol::ExitAction(Enemy* enemy){}
//
//// The functions for the chase state
//void Chase::EntryAction(Enemy* owner)
//{
//	owner->MoveTowards(owner->player->getPosition());
//}
//
//void Chase::Execute(Enemy* owner)
//{
//	if (owner->PlayerInSight())
//		owner->MoveForward(0.00075f);
//	else
//	{
//		Patrol* patrol = new Patrol();
//		owner->ChangeState(patrol);
//	}
//}
//
//void Chase::ExitAction(Enemy* enemy) {}