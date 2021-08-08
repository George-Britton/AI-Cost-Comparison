// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../CompositeNode.h"
#include "SequenceNode.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API USequenceNode : public UCompositeNode
{
	GENERATED_BODY()

public:
	USequenceNode() { NodeType = ENodeType::SEQUENCE; }

	// The current child index to execute
	int32 ChildIndex = 0;
	
	// The overridden sequence function
	void Sequence() override;
};
