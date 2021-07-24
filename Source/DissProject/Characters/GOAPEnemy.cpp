// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPEnemy.h"

// Sets default values
AGOAPEnemy::AGOAPEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGOAPEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGOAPEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to create a plan of actions
TArray<FAction> AGOAPEnemy::Formulate()
{
	TArray<FAction> outputAction;
	TArray<FGOAPState> outputState;
	outputState.Init(FGOAPState::FGOAPState(), 1);
	outputAction.Init(FAction::FAction(&outputState, EStateCase::MAX, &outputState), 1);
	return outputAction;
}

// Called to ensure the current plan is valid
bool AGOAPEnemy::ValidatePlan(TArray<FAction> TestPlan)
{
	return false;
}

// Called to bind functionality to input
void AGOAPEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

