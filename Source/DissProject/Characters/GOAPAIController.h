// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../Pickups/Weapon.h"
#include "GOAPAIController.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API AGOAPAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	// The parent enemy
	UPROPERTY(EditAnywhere, Category = "GOAP")
		class AGOAPEnemy* ParentEnemy = nullptr;

public:
	// Called to send the enemy to the location
	void MoveToLocation(FVector Location);

	// Called to shoot a laser
	void Attack(FWeaponDetails Weapon, FVector TargetLocation);
};
