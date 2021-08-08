// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../DecoratorNode.h"
#include "InverterNode.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API UInverterNode : public UDecoratorNode
{
	GENERATED_BODY()
	
public:
	UInverterNode() { NodeType = ENodeType::INVERTER; }
	
	// The overridden invert function from the parent
	void Invert() override;
};
