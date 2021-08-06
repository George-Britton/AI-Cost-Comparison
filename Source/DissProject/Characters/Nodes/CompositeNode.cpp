// Fill out your copyright notice in the Description page of Project Settings.


#include "CompositeNode.h"

// Overridden execute function
void UCompositeNode::Execute()
{
	switch (NodeType)
	{
	case ENodeType::SELECTOR: Select(); break;
	case ENodeType::SEQUENCE: Sequence(); break;
	default: break;
	}
}