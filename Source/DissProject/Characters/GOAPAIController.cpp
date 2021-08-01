// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPAIController.h"
#include "Laser.h"
#include "GOAPEnemy.h"
#include "Kismet/KismetMathLibrary.h"

// Called to send the enemy to the location
void AGOAPAIController::MoveToLocation(AActor* Actor)
{
	MoveToActor(Actor);
}

// Called to shoot a laser
void AGOAPAIController::Attack(FWeaponDetails Weapon)
{
	// We make sure we are facing the player
	ParentEnemy->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(ParentEnemy->GetActorLocation(), ParentEnemy->Player->GetActorLocation()));

	// These are the parameters for the laser
	FActorSpawnParameters SpawnParams;
	FVector SpawnLoc = ParentEnemy->GetActorLocation() + ParentEnemy->GetActorForwardVector() * 100;
	FRotator SpawnRot = FRotator(0, 0, 0);
	ALaser* Laser;

	// Spawn a laser and assign its attributes
	Laser = GetWorld()->SpawnActor<ALaser>(ALaser::StaticClass(), SpawnLoc, SpawnRot, SpawnParams);
	Laser->SetupLaser(Weapon.Range, Weapon.Damage, ParentEnemy->GetActorForwardVector(), 100.f);
	ParentEnemy->AttackSoundComponent->Play();
	ParentEnemy->AttackTimer = Weapon.RateOfFire;
	--ParentEnemy->CurrentWeapon.Ammo;
}