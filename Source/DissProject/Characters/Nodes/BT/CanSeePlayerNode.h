// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../LeafNode.h"
#include "CanSeePlayerNode.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API UCanSeePlayerNode : public ULeafNode
{
	GENERATED_BODY()
	
public:
	UCanSeePlayerNode() { NodeType = ENodeType::CONDITION; }
	
	// The condition function used to check if the enemy can see the player
	void Condition() override;
};
