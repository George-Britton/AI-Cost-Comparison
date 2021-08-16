// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackNode.h"
#include "../../Laser.h"
#include "../../BTEnemy.h"
#include "../../PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

// The action function of the attack node
void UAttackNode::Action()
{
	// We get the class of the parent enemy
	ABTEnemy* Enemy = Cast<ABTEnemy>(ParentEnemy);
	
	// We make sure we are facing the player
	Enemy->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Enemy->GetActorLocation(), Player->GetActorLocation()));
	
	// This check ensures that the enemy only spawns a laser if the player is far enough away that the laser will not bug out due to start overlaps
	if (UKismetMathLibrary::Vector_Distance(Enemy->GetActorLocation(), Player->GetActorLocation()) > 300.f)
	{
		// These are the parameters for the laser
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		FVector SpawnLoc = Enemy->GetActorLocation() + Enemy->GetActorForwardVector() * 100;
		FRotator SpawnRot = FRotator(0, 0, 0);

		// Spawn a laser and assign its attributes
		ALaser* Laser = GetWorld()->SpawnActor<ALaser>(ALaser::StaticClass(), SpawnLoc, SpawnRot, SpawnParams);
		if (Laser) Laser->SetupLaser(Enemy->CurrentWeapon.Range, Enemy->CurrentWeapon.Damage, Enemy->GetActorForwardVector(), 100.f);
	}
	else // If the player is too close to spawn a laser, then just hurt the player, but play the sound anyway to make it seem like they have been shot
	{
		Cast<APlayerCharacter>(Player)->RecieveAttack(Enemy->CurrentWeapon.Damage);
	}
	Enemy->AttackSoundComponent->Play();
	Enemy->AttackTimer = Enemy->CurrentWeapon.RateOfFire * 2;
	Enemy->CurrentWeapon.Ammo--;
	ParentNode->ChildrenStates[ParentNode->Children.Find(this)] = ENodeState::SUCCESS;
}