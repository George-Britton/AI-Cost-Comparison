// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../LeafNode.h"
#include "GoToNode.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API UGoToNode : public ULeafNode
{
	GENERATED_BODY()
	
public:
	UGoToNode() { NodeType = ENodeType::ACTION; }
	
	// The overridden action function to send the enemy to its goal location
	void Action() override;
};
