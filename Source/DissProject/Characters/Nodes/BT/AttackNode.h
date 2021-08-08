// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../LeafNode.h"
#include "AttackNode.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API UAttackNode : public ULeafNode
{
	GENERATED_BODY()
	
public:
	UAttackNode() { NodeType = ENodeType::ACTION; }
	
	// The action function of the attack node
	void Action() override;
};
