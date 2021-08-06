// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseNode.h"
#include "CompositeNode.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API UCompositeNode : public UBaseNode
{
	GENERATED_BODY()

public:
	// Overridden execute function
	void Execute() override;

	// The possible functions of the node
	virtual void Select() {}
	virtual void Sequence() {}
};
