// Fill out your copyright notice in the Description page of Project Settings.


#include "InverterNode.h"

// The overridden invert function from the parent
void UInverterNode::Invert()
{
	Children[0]->Execute();
	if (ChildrenStates[0] == ENodeState::SUCCESS) ParentNode->ChildrenStates[ParentNode->Children.Find(this)] = ENodeState::FAILURE;
	else if (ChildrenStates[0] == ENodeState::FAILURE) ParentNode->ChildrenStates[ParentNode->Children.Find(this)] = ENodeState::SUCCESS;
	else if (ChildrenStates[0] == ENodeState::RUNNING) ParentNode->ChildrenStates[ParentNode->Children.Find(this)] = ENodeState::RUNNING;
}