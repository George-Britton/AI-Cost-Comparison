// Fill out your copyright notice in the Description page of Project Settings.


#include "IsAtAmmoNode.h"
#include "../../BTEnemy.h"
#include "Kismet/KismetMathLibrary.h"

// The overridden condition functino to check if the enemy is at ammo
void UIsAtAmmoNode::Condition()
{
	if (UKismetMathLibrary::Vector_Distance(ParentEnemy->GetActorLocation(), Cast<ABTEnemy>(ParentEnemy)->MoveToTarget->GetActorLocation()) <= 100.f) ParentNode->ChildrenStates[0] = ENodeState::SUCCESS;
	else ParentNode->ChildrenStates[0] = ENodeState::RUNNING;
}