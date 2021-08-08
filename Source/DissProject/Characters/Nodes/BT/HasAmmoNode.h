// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../LeafNode.h"
#include "HasAmmoNode.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API UHasAmmoNode : public ULeafNode
{
	GENERATED_BODY()

public:
	UHasAmmoNode() { NodeType = ENodeType::CONDITION; }
	
	// The current inventory item of the enemy
	int32 CurrentInventoryItem = 0;
	
	// The condition function to check for ammo
	void Condition() override;
};
