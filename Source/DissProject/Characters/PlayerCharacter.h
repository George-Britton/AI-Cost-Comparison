// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Pickups/PickupBase.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

// We create a new delegate type the debug message function
DECLARE_DELEGATE_OneParam(FDebugDelegate, FString);
// We create delegate for the toggling of boolean states
DECLARE_DELEGATE_OneParam(FToggleState, bool);
// We make a delegate for when the player takes damage
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamage, float, NewHealth);
// We also make a delegate for when the action button is pressed
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionPressed);
// We announce a change in wealth
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyUpdate, int32, NewMoney);
// We announce that a weapon has been drawn
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponDraw, bool, WeaponDrawn);
// We finally make a delegate for if the player dies
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);

UCLASS()
class DISSPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Used to determine how much life the player has
	UPROPERTY(EditAnywhere, Category = "Life")
		float Health = 100.f;
	UPROPERTY(EditAnywhere, Category = "Life")
		USoundBase* DamageSound = nullptr;
	UPROPERTY()
		UAudioComponent* DamageSoundComponent = nullptr;
	UPROPERTY(BlueprintAssignable, Category = "Life")
		FOnDamage OnDamage;
	UPROPERTY(BlueprintAssignable, Category = "Life")
		FOnPlayerDeath OnPlayerDeath;

	// Variables for the player's camera
	UPROPERTY()
		UCameraComponent* PlayerCamera = nullptr;
	UPROPERTY(EditAnywhere, Category = "Camera")
		float CameraHeight = 80.f;
	UPROPERTY(EditAnywhere, Category = "Camera")
		float CameraPitchLimit = 70.f;

	// Variables for the player's movement
	UPROPERTY(EditAnywhere, Category = "Movement")
		float RunSpeed = 1050.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float CrouchSpeed = 200.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float BaseSpeed = 600.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float JumpHeight = 250.f;
	UPROPERTY()
		bool IsRunning = false;
	UPROPERTY()
		bool IsCrouching = false;

	// Variables for the overlapping pickups
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
		APickupBase* OverlappedPickup = nullptr;

	// Variables related to the player's inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 CurrentMoney = 0;

	// Event distributors
	// Used to announce that the action button has been pressed
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnActionPressed OnActionPressed;
	// Used to announce that the player has gained or lost money
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnMoneyUpdate OnMoneyUpdate;
	// Used to announce that the player has changed weapon
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnWeaponDraw OnWeaponDraw;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when a value changes
	void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// INPUTS
	// Makes the player move left or right
	void MoveX(float AxisValue);
	// Makes the player move forward or backward
	void MoveY(float AxisValue);
	// Makes the player look left or right
	void RotateX(float AxisValue);
	// Makes the player look up or down
	void RotateY(float AxisValue);
	// Makes the player run
	void ToggleRun(bool Running);
	// Crouches the player down
	void ToggleCrouch(bool Crouching);
	// Announces the action button is pressed
	void Action();

	// EFFECTS
	// Used to tell the world that money has been collected
	void UpdateMoney(int32 Money);
	// Used to tell the world a weapon has been drawn
	void DrawWeapon();

	// ATTACKS
	// Used to tell the player they have been attacked by a zombie
	void RecieveAttack(float Damage);

	// DEBUG
	// This function is used to print errors that occur during runtime
	UFUNCTION()
		void PrintDebugMessage(FString DebugMessage) { GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, DebugMessage); };
};