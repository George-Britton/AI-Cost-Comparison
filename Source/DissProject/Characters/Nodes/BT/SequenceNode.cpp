// Fill out your copyright notice in the Description page of Project Settings.


#include "SequenceNode.h"

// The overridden sequence function
void USequenceNode::Sequence()
{
	Children[ChildIndex]->Execute();
	
	if (ChildrenStates[ChildIndex] == ENodeState::SUCCESS) // We start the next child as running and execute it if the current is done ...
	{
		++ChildIndex;
		if (ChildIndex == Children.Num()) ChildIndex = 0;
		ChildrenStates[ChildIndex] = ENodeState::RUNNING;
	} 
	else if (ChildrenStates[ChildIndex] == ENodeState::FAILURE && ParentNode) // ... or we end the sequence and tell the parent it is finished if the current failed
	{
		ParentNode->ChildrenStates[ParentNode->Children.Find(this)] = ENodeState::SUCCESS;
	}
}