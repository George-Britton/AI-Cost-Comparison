// Fill out your copyright notice in the Description page of Project Settings.


#include "GetClosestAmmoNode.h"
#include "../../BTEnemy.h"
#include "Kismet/KismetMathLibrary.h"

// The overridden condition function that finds the closest ammo
void UGetClosestAmmoNode::Condition()
{
	ABTEnemy* Enemy = Cast<ABTEnemy>(ParentEnemy);
	AActor* ClosestSpawner = nullptr;
	
	for (auto& Spawner : Enemy->WeaponSpawners)
	{
		if (ClosestSpawner)
		{
			if (UKismetMathLibrary::Vector_Distance(Spawner->GetActorLocation(), Enemy->GetActorLocation()) < UKismetMathLibrary::Vector_Distance(ClosestSpawner->GetActorLocation(), Enemy->GetActorLocation()))
				ClosestSpawner = Spawner;
		}
		else ClosestSpawner = Spawner;
	}
	Enemy->MoveToTarget = ClosestSpawner;
	ParentNode->ChildrenStates[ParentNode->Children.Find(this)] = ENodeState::SUCCESS;
}