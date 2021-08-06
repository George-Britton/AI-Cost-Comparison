// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseNode.h"
#include "DecoratorNode.generated.h"

UENUM()
enum class ENodeState : uint8
{
	SUCCESS UMETA(DisplayName = "Success"),
	FAILURE UMETA(DisplayName = "Failure"),
	RUNNING UMETA(DisplayName = "Running"),
	MAX
};

UCLASS()
class DISSPROJECT_API UDecoratorNode : public UBaseNode
{
	GENERATED_BODY()

		// The state of the child of this node
		ENodeState ChildState = ENodeState::MAX;

public:
	// Overridden execute function
	void Execute() override;

	// The possible functions of the node
	virtual void Invert() {}
	virtual void Success() {}
	virtual void Failure() {}
	virtual void Repeat() {}
	virtual void Retry() {}
};
