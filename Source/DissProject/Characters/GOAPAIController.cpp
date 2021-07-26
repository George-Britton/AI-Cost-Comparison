// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPAIController.h"
#include "Laser.h"
#include "GOAPEnemy.h"

// Called to send the enemy to the location
void AGOAPAIController::MoveToLocation(FVector Location)
{
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(Location);
	MoveRequest.SetAcceptanceRadius(5.f);
	MoveTo(MoveRequest);
}

// Called to shoot a laser
void AGOAPAIController::Attack(FWeaponDetails Weapon, FVector TargetLocation)
{
	// These are the parameters for the ray trace
	FHitResult HitEnemy;
	FVector RayStart = ParentEnemy->GetActorLocation();
	FVector RayEnd = RayStart + (ParentEnemy->GetActorForwardVector() * Weapon.Range);
	FCollisionQueryParams CollisionParameters;
	CollisionParameters.AddIgnoredActor(ParentEnemy);

	// These are the parameters for the laser
	FActorSpawnParameters SpawnParams;
	FVector SpawnLoc = ParentEnemy->GetActorLocation() + ParentEnemy->GetActorForwardVector() * 100;
	FRotator SpawnRot = FRotator(0, 0, 0);
	ALaser* Laser;

	switch(Weapon.Type)
	{
	case EWeaponType::RANGED:
		// If the weapon is ranged, spawn a laser and assign its attributes
		Laser = GetWorld()->SpawnActor<ALaser>(ALaser::StaticClass(), SpawnLoc, SpawnRot, SpawnParams);
		Laser->SetupLaser(Weapon.Range, Weapon.Damage, ParentEnemy->GetActorForwardVector(), 100.f);
		ParentEnemy->AttackSoundComponent->SetSound(ParentEnemy->RangedSound);
		ParentEnemy->AttackSoundComponent->Play();
		ParentEnemy->AttackTimer = Weapon.RateOfFire;
		--ParentEnemy->CurrentWeapon.Ammo;
		break;
	case EWeaponType::MELEE:
		// If the weapon is melee, ray trace in front of the enemy and see if it hits the player


		ParentEnemy->AttackSoundComponent->SetSound(ParentEnemy->RangedSound);
		ParentEnemy->AttackSoundComponent->Play();
		ParentEnemy->AttackTimer = Weapon.RateOfFire;
		break;
	default: break;
	}
}