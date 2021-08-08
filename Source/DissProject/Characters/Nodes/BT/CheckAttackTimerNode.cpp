// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckAttackTimerNode.h"
#include "../../BTEnemy.h"

// The condition function used to check if the enemy's attack timer is cooled down
void UCheckAttackTimerNode::Condition()
{
	if (Cast<ABTEnemy>(ParentEnemy)->AttackTimer <= 0.f) ParentNode->ChildrenStates[0] = ENodeState::SUCCESS;
	else ParentNode->ChildrenStates[0] = ENodeState::RUNNING;
}