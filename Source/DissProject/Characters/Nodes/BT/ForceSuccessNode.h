// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../DecoratorNode.h"
#include "ForceSuccessNode.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API UForceSuccessNode : public UDecoratorNode
{
	GENERATED_BODY()
	
public:
	UForceSuccessNode() { NodeType = ENodeType::SUCCESS; }
	
	// The overridden force success function from the parent
	void Success() override;
};
