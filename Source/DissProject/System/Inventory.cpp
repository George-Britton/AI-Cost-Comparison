// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called to add a pickup
void UInventory::AddItem(FWeaponDetails InWeapon, uint8 InMoney)
{
	if (InMoney == 0)
	{
		// We first loop through the inventory to see if we already have the weapon
		if (Inventory.Num() > 0)
		{
			bool HasWeapon = false;
			for (auto& Weapon : Inventory)
			{
				if (Weapon.Name == InWeapon.Name)
				{
					// if the weapon is ranged, just increase the ammo
					if (Weapon.Type == EWeaponType::RANGED)
					{
						Weapon.Ammo += InWeapon.Ammo;
					}

					// Mark the flag as true
					HasWeapon = true;
					break;
				}
			}
			if (!HasWeapon) Inventory.Add(FWeaponDetails::FWeaponDetails(InWeapon));
		}
		else Inventory.Add(FWeaponDetails::FWeaponDetails(InWeapon));
	}

	// Add the input money to the owned money, doesn't matter if 0
	Money += InMoney;
}