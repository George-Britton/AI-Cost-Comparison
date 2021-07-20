// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "Money.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API AMoney : public APickupBase
{
	GENERATED_BODY()
	
public:
	// Default constructor
	AMoney();

	// The value of the money pickup
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Value")
		int32 Value = 10;

public:
	// Overridden activate function
	void Activate() override;
};
