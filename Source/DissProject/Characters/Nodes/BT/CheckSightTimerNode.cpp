// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckSightTimerNode.h"
#include "../../BTEnemy.h"

// The condition function used to check if the enemy's sight timer is cooled down
void UCheckSightTimerNode::Condition()
{
	if (Cast<ABTEnemy>(ParentEnemy)->SightTimer <= 0.f) ParentNode->ChildrenStates[0] = ENodeState::SUCCESS;
	else ParentNode->ChildrenStates[0] = ENodeState::RUNNING;
}