// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

	// The actor that we are checking the state of
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		AActor* Subject;

	// The bool value we might check
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		bool BoolValue;

	// The float value we might check
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		float FloatValue;

	// The int value we might check
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		int32 IntValue;

	// The string value we might check
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		FString StringValue;

	// The vector value we might check
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		FVector VectorValue;

	// Default constructor
	FGOAPState(EVariableType InVariableType = EVariableType::MAX, AActor* InSubject = nullptr, bool InBoolValue = false, float InFloatValue = 0.f, int32 InIntValue = 0, FString InStringValue = "", FVector InVectorValue = FVector(0.f, 0.f, 0.f))
	{
		VariableType = InVariableType;
		Subject = InSubject;
		BoolValue = InBoolValue;
		FloatValue = InFloatValue;
		IntValue = InIntValue;
		StringValue = InStringValue;
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
	bool operator==(FString InString)
	{
		return StringValue == StringValue;
	}
	bool operator==(FVector InVector)
	{
		return VectorValue == VectorValue;
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

	// The world variables that must be true to undertake the action
	UPROPERTY(VisibleAnywhere, Category = "GOAP")
		TArray<FGOAPState> Preconditions;

	FAction()
	{}

	// Default constructor
	FAction(TArray<FGOAPState>* InEffects, EStateCase InStateCase = EStateCase::MAX, TArray<FGOAPState>* InPreconditions = nullptr)
	{
		Effects = *InEffects;
		StateCase = InStateCase;
		Preconditions = *InPreconditions;
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to create a plan of actions
	TArray<FAction> Formulate();

	// Called to ensure the current plan is valid
	bool ValidatePlan(TArray<FAction> TestPlan);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
