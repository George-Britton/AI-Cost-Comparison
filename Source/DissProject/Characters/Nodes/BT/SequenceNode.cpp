// Fill out your copyright notice in the Description page of Project Settings.


#include "SequenceNode.h"

// The overridden sequence function
void USequenceNode::Sequence()
{
	if (ChildrenStates[ChildIndex] == ENodeState::RUNNING) Children[ChildIndex]->Execute(); // We either continue executing the running child node...
	else if (ChildrenStates[ChildIndex] == ENodeState::SUCCESS) // ... start the next child as running and execute it...
	{
		++ChildIndex;
		if (ChildIndex == Children.Num()) ChildIndex = 0;
		ChildrenStates[ChildIndex] = ENodeState::RUNNING;
		Children[ChildIndex]->Execute();
	} // ... or we end the sequence and tell the parent it is finished
	else if (ChildrenStates[ChildIndex] == ENodeState::FAILURE && ParentNode) ParentNode->ChildrenStates[ParentNode->Children.Find(this)] = ENodeState::SUCCESS;
}