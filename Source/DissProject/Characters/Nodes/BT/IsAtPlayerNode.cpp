// Fill out your copyright notice in the Description page of Project Settings.


#include "IsAtPlayerNode.h"
#include "../../BTEnemy.h"
#include "Kismet/KismetMathLibrary.h"

// The overridden condition functino to check if the enemy is at the player
void UIsAtPlayerNode::Condition()
{
	ABTEnemy* Enemy = Cast<ABTEnemy>(ParentEnemy);
	if (UKismetMathLibrary::Vector_Distance(Enemy->GetActorLocation(), Player->GetActorLocation()) <= (Enemy->CurrentWeapon.Range * 0.7f))
	{
		Enemy->AIController->StopMovement();
		ParentNode->ChildrenStates[0] = ENodeState::SUCCESS;
	}
	else ParentNode->ChildrenStates[0] = ENodeState::RUNNING;
}