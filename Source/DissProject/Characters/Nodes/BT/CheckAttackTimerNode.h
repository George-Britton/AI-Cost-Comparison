// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../LeafNode.h"
#include "CheckAttackTimerNode.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API UCheckAttackTimerNode : public ULeafNode
{
	GENERATED_BODY()
	
public:
	UCheckAttackTimerNode() { NodeType = ENodeType::CONDITION; }

	// The condition function used to check if the enemy's attack timer is cooled down
	void Condition() override;
};
