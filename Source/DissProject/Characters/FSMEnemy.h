// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AIController.h"
#include "PlayerCharacter.h"
#include "GameFramework/Character.h"
#include "../Pickups/Weapon.h"
#include "../System/Inventory.h"
#include "FSMEnemy.generated.h"

// We make a delegate for if the enemy dies
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UENUM()
enum class EFSMState : uint8
{
	IDLE UMETA(DisplayName="Idle"),
	ATTACKING UMETA(DisplayName = "Attacking"),
	MOVINGTOPLAYER UMETA(DisplayName = "Moving to player"),
	MOVINGTOAMMO UMETA(DisplayName = "Moving to ammo"),
	MAX
};

UCLASS()
class DISSPROJECT_API AFSMEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFSMEnemy();

	// The health of the enemy
	UPROPERTY(EditAnywhere, Category = "FSM")
		float Health = 100.f;

	// The weapon currently drawn by the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		FWeaponDetails CurrentWeapon;
	// The array of weapons the enemy has
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		UInventory* Inventory = nullptr;
	// The index of the current weapon in the inventory
	int32 CurrentInventoryItem = 0;

	// The list of weapon spawners that the enemy can run to
	UPROPERTY(EditAnywhere, Category = "GOAP")
		TArray<AActor*> WeaponSpawners;

	// A reference to the player
	UPROPERTY(EditAnywhere, Category = "GOAP")
		APlayerCharacter* Player = nullptr;

	// The AI controller for the enemy
	AAIController* AIController = nullptr;
	EFSMState EnemyState = EFSMState::MOVINGTOPLAYER;

	// The frequency with which to check if the enemy can see the player, as every frame would be too much
	float SightTime = 0.5f;
	float SightTimer = 0.f;
	
	// Variables used for the enemy's attacks
	UPROPERTY()
		USkeletalMeshComponent* RangedMeshComponent = nullptr;
	UPROPERTY()
		float AttackTimer = 0.f;
	UAudioComponent* AttackSoundComponent = nullptr;
	USoundBase* RangedSound = nullptr;
	
	// Event distributors
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnDeath OnDeath;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called to create the inventory
	void CreateInventory();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to equip a weapon
	void Equip(FWeaponDetails Weapon);
	UFUNCTION(BlueprintCallable, Category = "Events")
		void PickupWeapon(FWeaponDetails WeaponDetails);

	// Used to see if the player is in line of sight
	bool CanSeePlayer();
	
	// Called to attack the player
	void Attack();

	// Called to get the closest weapon spawner for the enemy to go to
	AActor* GetClosestSpawner();
	
	// Called to inflict damage on the enemy
	void RecieveAttack(float Damage);
};
