// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../LeafNode.h"
#include "IsAtAmmoNode.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API UIsAtAmmoNode : public ULeafNode
{
	GENERATED_BODY()
	
public:
	UIsAtAmmoNode() { NodeType = ENodeType::CONDITION; }
	
	// The overridden condition functino to check if the enemy is at ammo
	void Condition() override;
};
