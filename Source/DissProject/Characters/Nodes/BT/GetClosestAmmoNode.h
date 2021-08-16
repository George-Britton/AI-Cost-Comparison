// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../LeafNode.h"
#include "GetClosestAmmoNode.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API UGetClosestAmmoNode : public ULeafNode
{
	GENERATED_BODY()
	
public:
	UGetClosestAmmoNode() { NodeType = ENodeType::CONDITION; }
	
	// The overridden condition function that finds the closest ammo
	void Condition() override;
};
