// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseNode.generated.h"

UENUM()
enum class ENodeType : uint8
{
	SEQUENCE UMETA(DisplayName = "Sequence"),
	INVERTER UMETA(DisplayName = "Inverter"),
	SUCCESS UMETA(DisplayName = "Success"),
	CONDITION UMETA(DisplayName = "Condition"),
	ACTION UMETA(DisplayName = "Action"),
	MAX
};

UENUM()
enum class ENodeState : uint8
{
	SUCCESS UMETA(DisplayName = "Success"),
	FAILURE UMETA(DisplayName = "Failure"),
	RUNNING UMETA(DisplayName = "Running"),
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

	// The parent enemy of the behaviour tree
	AActor* ParentEnemy = nullptr;

	// A reference to the player
	AActor* Player = nullptr;

	// The type of node that it is
	ENodeType NodeType = ENodeType::MAX;

	// The parent node of this node
	UBaseNode* ParentNode = nullptr;
	
	// The children of the node
	TArray<UBaseNode*> Children;
	TArray<ENodeState> ChildrenStates;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called to set the world variables from the parent enemies
	void InitNode(UBaseNode* InParentNode = nullptr, AActor* InParentEnemy = nullptr, AActor* InPlayer = nullptr);
	
	// Call upon the logic of the node
	virtual void Execute() {};
		
};
