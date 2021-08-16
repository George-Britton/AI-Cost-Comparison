// Fill out your copyright notice in the Description page of Project Settings.


#include "DecoratorNode.h"

// Overridden execute function
void UDecoratorNode::Execute()
{
	Super::Execute();
	
	switch (NodeType)
	{
	case ENodeType::INVERTER: Invert(); break;
	case ENodeType::SUCCESS: Success(); break;
	default: break;
	}
}

// The possible functions of the node
void UDecoratorNode::Invert()
{
	
}
void UDecoratorNode::Success()
{
	
}