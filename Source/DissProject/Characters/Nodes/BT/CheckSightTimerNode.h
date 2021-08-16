// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../LeafNode.h"
#include "../../BTEnemy.h"
#include "CheckSightTimerNode.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API UCheckSightTimerNode : public ULeafNode
{
	GENERATED_BODY()
	
public:
	UCheckSightTimerNode() { NodeType = ENodeType::CONDITION; }
	
	// The condition function used to check if the enemy's sight timer is cooled down
	void Condition() override;
};
