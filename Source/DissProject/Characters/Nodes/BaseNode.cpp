// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseNode.h"

// Sets default values for this component's properties
UBaseNode::UBaseNode()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBaseNode::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called to set the world variables from the parent enemies
void UBaseNode::InitNode(UBaseNode* InParentNode, AActor* InParentEnemy, AActor* InPlayer)
{
	// Set this node's enemy and player variables
	ParentNode = InParentNode;
	ParentEnemy = InParentEnemy;
	Player = InPlayer;

	// Traverse down the tree and initiate the children
	if (Children.IsValidIndex(0))
	{
		for (auto& Child : Children) Child->InitNode(this, InParentEnemy, InPlayer);
		
		// Set the children state array to have the same number of states as children
		ChildrenStates.Init(ENodeState::MAX, Children.Num());
	}
}
