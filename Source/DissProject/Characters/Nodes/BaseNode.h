// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseNode.generated.h"

UENUM()
enum class ENodeType : uint8
{
	SEQUENCE UMETA(DisplayName = "Sequence"),
	SELECTOR UMETA(DisplayName = "Selector"),
	INVERTER UMETA(DisplayName = "Inverter"),
	SUCCESS UMETA(DisplayName = "Success"),
	FAILURE UMETA(DisplayName = "Failure"),
	REPEATER UMETA(DisplayName = "Repeater"),
	RETRY UMETA(DisplayName = "Retry"),
	CONDITION UMETA(DisplayName = "Condition"),
	ACTION UMETA(DisplayName = "Action"),
	MAX
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DISSPROJECT_API UBaseNode : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseNode();
	
	// The name of the node
	FName Name = "";

	// The type of node that it is
	ENodeType NodeType = ENodeType::MAX;

	// The children of the node
	TArray<UBaseNode*> Children;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Call upon the logic of the node
	virtual void Execute() {};
		
};
