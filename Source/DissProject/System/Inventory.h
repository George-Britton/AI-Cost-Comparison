// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Pickups/Weapon.h"
#include "Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DISSPROJECT_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventory();

	// The array of items the character can hold
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TArray<FWeaponDetails> Inventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		uint8 Money = 0;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called to add a pickup
	void AddItem(FWeaponDetails InWeapon, uint8 InMoney = 0);
};
