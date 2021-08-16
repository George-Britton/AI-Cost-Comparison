// Fill out your copyright notice in the Description page of Project Settings.


#include "CanSeePlayerNode.h"
#include "../../Laser.h"
#include "../../BTEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// The condition function used to check if the enemy can see the player
void UCanSeePlayerNode::Condition()
{
	// Get the parent enemy's class
	ABTEnemy* Enemy = Cast<ABTEnemy>(ParentEnemy);
	
	// These are the parameters for the ray trace
	FHitResult HitActor;
	FVector RayStart = ParentEnemy->GetActorLocation();
	FCollisionQueryParams CollisionParameters;

	// Add all the ignored actors for the check, including all enemies, weapon spawners, and lasers
	TArray<AActor*> AllLasers;
	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALaser::StaticClass(), AllLasers);
	CollisionParameters.AddIgnoredActor(Enemy);
	CollisionParameters.AddIgnoredActors(Enemy->WeaponSpawners);
	CollisionParameters.AddIgnoredActors(AllLasers);

	FVector PlayerLocation = Player->GetActorLocation();
	// Here we do a ray trace to see if the player is in line of sight
	if (GetWorld()->LineTraceSingleByChannel(HitActor, RayStart, PlayerLocation, ECC_Visibility, CollisionParameters))
	{
		// We check to see if the hit location is within an acceptable distance of the target to allow for volume boundries
		if (HitActor.Actor == Player && UKismetMathLibrary::Vector_Distance(HitActor.Location, ParentEnemy->GetActorLocation()) < Enemy->CurrentWeapon.Range * 0.8f)
		{
			ParentNode->ChildrenStates[0] = ENodeState::SUCCESS;
		}
		else
		{
			Enemy->MoveToTarget = Player;
			ParentNode->ChildrenStates[0] = ENodeState::FAILURE;
		}
	}
	else
	{
		Enemy->MoveToTarget = Player;
		ParentNode->ChildrenStates[0] = ENodeState::FAILURE;
	}
}