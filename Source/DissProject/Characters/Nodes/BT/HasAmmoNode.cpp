// Fill out your copyright notice in the Description page of Project Settings.


#include "HasAmmoNode.h"
#include "../../BTEnemy.h"

// The condition function to check for ammo
void UHasAmmoNode::Condition()
{
	ABTEnemy* Enemy = Cast<ABTEnemy>(ParentEnemy);
	
	if (Enemy->CurrentWeapon.Ammo == 0)
	{
		// First we save the current weapon
		Enemy->Inventory->Inventory[CurrentInventoryItem].Ammo = 0;

		// We loop through the inventory and find anything with ammo
		bool AmmoFound = false;
		for (int32 InventoryIndex = 0; InventoryIndex < Enemy->Inventory->Inventory.Num(); ++InventoryIndex)
		{
			if (Enemy->Inventory->Inventory[InventoryIndex].Ammo > 0)
			{
				AmmoFound = true;
				CurrentInventoryItem = InventoryIndex;
				break;
			}
		}

		// If we found ammo, equip that weapon
		if (AmmoFound)
		{
			Enemy->Equip(Enemy->Inventory->Inventory[CurrentInventoryItem]);
			ParentNode->ChildrenStates[0] = ENodeState::SUCCESS;
		}
		else
		{
			ParentNode->ChildrenStates[0] = ENodeState::FAILURE;
		}
	}
	else
	{
		ParentNode->ChildrenStates[0] = ENodeState::SUCCESS;
	}
}
