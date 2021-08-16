// Fill out your copyright notice in the Description page of Project Settings.


#include "ForceSuccessNode.h"

// The overridden invert function from the parent
void UForceSuccessNode::Success()
{
	Children[0]->Execute();
	if (ChildrenStates[0] != ENodeState::FAILURE) ParentNode->ChildrenStates[ParentNode->Children.Find(this)] = ChildrenStates[0];
}