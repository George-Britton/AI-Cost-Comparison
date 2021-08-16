// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../LeafNode.h"
#include "IsAtPlayerNode.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API UIsAtPlayerNode : public ULeafNode
{
	GENERATED_BODY()
	
public:
	UIsAtPlayerNode() { NodeType = ENodeType::CONDITION; }
	
	// The overridden condition functino to check if the enemy is at the player
	void Condition() override;
};
