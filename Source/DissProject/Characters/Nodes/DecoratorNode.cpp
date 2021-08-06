// Fill out your copyright notice in the Description page of Project Settings.


#include "DecoratorNode.h"

// Overridden execute function
void UDecoratorNode::Execute()
{
	switch (NodeType)
	{
	case ENodeType::INVERTER: Invert(); break;
	case ENodeType::SUCCESS: Success(); break;
	case ENodeType::FAILURE: Failure(); break;
	case ENodeType::REPEATER: Repeat(); break;
	case ENodeType::RETRY: Retry(); break;
	default: break;
	}
}