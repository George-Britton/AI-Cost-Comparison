// Fill out your copyright notice in the Description page of Project Settings.


#include "LeafNode.h"

// Overridden execute function
void ULeafNode::Execute()
{
	switch (NodeType)
	{
	case ENodeType::CONDITION: Condition(); break;
	case ENodeType::ACTION: Action(); break;
	default: break;
	}
}