// Fill out your copyright notice in the Description page of Project Settings.


#include "GoToNode.h"
#include "../../BTEnemy.h"

// The overridden action function to send the enemy to its goal location
void UGoToNode::Action()
{
	ABTEnemy* Enemy = Cast<ABTEnemy>(ParentEnemy);
	Enemy->AIController->MoveToActor(Enemy->MoveToTarget, 20.f, false);
	ParentNode->ChildrenStates[ParentNode->Children.Find(this)] = ENodeState::SUCCESS;
}