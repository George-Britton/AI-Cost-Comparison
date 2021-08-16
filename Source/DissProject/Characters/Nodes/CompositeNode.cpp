// Fill out your copyright notice in the Description page of Project Settings.


#include "CompositeNode.h"

// Overridden execute function
void UCompositeNode::Execute()
{
	Super::Execute();
	
	Sequence();
}

// The sequence function of the node
void UCompositeNode::Sequence()
{
	
}