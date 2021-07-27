// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../System/Inventory.h"
#include "../Pickups/Weapon.h"
#include "GOAPAIController.h"
#include "Components/AudioComponent.h"
#include "GOAPEnemy.generated.h"

// Enum to tell the state what variable type it should be checking for true
UENUM()
enum class EVariableType : uint8
{
	BOOLEAN UMETA(DisplayName="Boolean"),
	FLOAT UMETA(DisplayName = "Float"),
	INTEGER UMETA(DisplayName = "Integer"),
	STRING UMETA(DisplayName = "String"),
	VECTOR UMETA(DisplayName = "Vector"),
	MAX
};

// World state queries that the state may be checking
UENUM()
enum class EStateCase : uint8
{
	SIGHT UMETA(DisplayName = "Can see"),
	LOCATION UMETA(DisplayName = "Is at location"),
	HEALTH UMETA(DisplayName = "Health value"),
	AMMO UMETA(DisplayName = "Ammunition"),
	MAX
};

// Struct of desirable world state configurations
USTRUCT()
struct FGOAPState
{
	GENERATED_BODY()

	// The type of variable to check against
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		EVariableType VariableType;

	// The world query done to test the goal state
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		EStateCase StateCase;

	// The actor that we are checking the state of
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		FString Subject;

	// The bool value we might check
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		bool BoolValue;

	// The float value we might check
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		float FloatValue;

	// The int value we might check
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		int32 IntValue;

	// The vector value we might check
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		FVector VectorValue;

	// Default constructor
	FGOAPState(EVariableType InVariableType = EVariableType::MAX, EStateCase InStateCase = EStateCase::MAX, FString InSubject = "", bool InBoolValue = false, float InFloatValue = 0.f, int32 InIntValue = 0, FVector InVectorValue = FVector(0.f, 0.f, 0.f))
	{
		VariableType = InVariableType;
		StateCase = InStateCase,
		Subject = InSubject;
		BoolValue = InBoolValue;
		FloatValue = InFloatValue;
		IntValue = InIntValue;
		VectorValue = InVectorValue;
	}

	// Returns if the passed in value is less than the set value of the state
	bool operator<(float InFloat)
	{
		return InFloat < FloatValue;
	}
	bool operator<(int32 InInt)
	{
		return InInt < IntValue;
	}

	// Returns if the passed in value is less than or equal to the set value of the state
	bool operator<=(float InFloat)
	{
		return InFloat <= FloatValue;
	}
	bool operator<=(int32 InInt)
	{
		return InInt <= IntValue;
	}

	// Returns if the passed in value is greater than the set value of the state
	bool operator>(float InFloat)
	{
		return InFloat > FloatValue;
	}
	bool operator>(int32 InInt)
	{
		return InInt > IntValue;
	}

	// Returns if the passed in value is greater than or equal to the set value of the state
	bool operator>=(float InFloat)
	{
		return InFloat >= FloatValue;
	}
	bool operator>=(int32 InInt)
	{
		return InInt >= IntValue;
	}

	// Returns if the passed in value is equal to the set value of the state
	bool operator==(bool InBool)
	{
		return BoolValue == BoolValue;
	}
	bool operator==(float InFloat)
	{
		return FloatValue == FloatValue;
	}
	bool operator==(int32 InInt)
	{
		return IntValue == IntValue;
	}
	bool operator==(FVector InVector)
	{
		return VectorValue == VectorValue;
	}

	// Returns if the passed in state is the same as the tested one
	bool operator==(FGOAPState InState)
	{
		return (VariableType == InState.VariableType &&
				StateCase == InState.StateCase &&
				Subject == InState.Subject &&
				BoolValue == InState.BoolValue &&
				FloatValue == InState.FloatValue &&
				IntValue == InState.IntValue &&
				VectorValue == InState.VectorValue);
	}
	bool operator!=(FGOAPState InState)
	{
		return !(VariableType == InState.VariableType &&
				StateCase == InState.StateCase &&
				Subject == InState.Subject &&
				BoolValue == InState.BoolValue &&
				FloatValue == InState.FloatValue &&
				IntValue == InState.IntValue &&
				VectorValue == InState.VectorValue);
	}
};

// The struct of actions the enemy can do to affect the game world
USTRUCT()
struct FAction
{
	GENERATED_BODY()

	// The world variables that the action will make true
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		TArray<FGOAPState> Effects;

	// The world query done to test the goal state
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		EStateCase StateCase;

	// The action state to take in order to achieve the goal
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		FString Action;

	// The world variables that must be true to undertake the action
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		TArray<FGOAPState> Preconditions;

	// Default constructor
	FAction(TArray<FGOAPState> InEffects = TArray<FGOAPState>::TArray(), EStateCase InStateCase = EStateCase::MAX, FString InAction = "", TArray<FGOAPState> InPreconditions = TArray<FGOAPState>::TArray())
	{
		if (InEffects.IsValidIndex(0))
		{
			for (int32 Index = 0; Index < InEffects.Num(); ++Index)
			{
				Effects.Emplace(InEffects[Index]);
			}
		}
		StateCase = InStateCase;
		Action = InAction;
		if (InPreconditions.IsValidIndex(0))
		{
			for (int32 Index = 0; Index < InPreconditions.Num(); ++Index)
			{
				Preconditions.Emplace(InPreconditions[Index]);
			}
		}
	}
};

UCLASS()
class DISSPROJECT_API AGOAPEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGOAPEnemy();

	// The array of actions the enemy can use
	UPROPERTY(EditAnywhere, Category = "GOAP")
		TArray<FAction> Actions;

	// The formulated plan for the enemy to use to reach their goal. The order of progression is descending from Plan.Num() - 1
	UPROPERTY(EditAnywhere, Category = "GOAP")
		TArray<FAction> Plan;

	// The health of the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
		float Health = 100.f;

	// The acceptable error margin of a ray trace for sight
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
		float LocationErrorMargin = 100.f;

	// The weapon currently drawn by the enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
		FWeaponDetails CurrentWeapon;
	// The array of weapons the enemy has
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
		UInventory* Inventory = nullptr;
	// The meshes of the weapons
	UStaticMeshComponent* MeleeMesh = nullptr;
	USkeletalMeshComponent* RangedMesh = nullptr;

	// The audio of the attacks
	UAudioComponent* AttackSoundComponent = nullptr;
	USoundBase* MeleeSound = nullptr;
	USoundBase* RangedSound = nullptr;

	// The AI controller for the enemy
	AGOAPAIController* GOAPController = nullptr;

	// Variables used for the enemy's attacks
	UPROPERTY()
		USkeletalMeshComponent* RangedMeshComponent = nullptr;
	UPROPERTY()
		bool Attacking = false;
	UPROPERTY()
		float AttackTimer = 0.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to create the inventory
	void CreateInventory();

	// Called to create the actionbase
	void CreateActionBase();

	// Called to create a plan of actions
	TArray<FAction> GetPlan();
	TArray<FAction> Formulate(int32 Steps, FGOAPState Precondition, TArray<FAction> CurrentPlan);

	// Called to ensure the current plan is valid
	bool ValidatePlan(TArray<FAction> TestPlan);

	// Called when an enemy wants to initiate an action, returns whether the action is valid
	bool TakeAction(FAction Action);

	// Called to validate an action's precondition is met
	bool ValidatePrecondition(FGOAPState Precondition);

	// Called to query the world state
	// Checks line of sight
	bool CheckSight(FVector LookAtLocation, FString Actor = "");
	// Checks if actor is at location
	bool CheckLocation(FVector IsAtLocation);
	// Checks the health of the given actor, returns true if below or equal to given float
	bool CheckHealth(float InHealth, FString InSubject);
	// Checks the ammo of the enemy, returns true if has ammo
	bool CheckAmmo();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
