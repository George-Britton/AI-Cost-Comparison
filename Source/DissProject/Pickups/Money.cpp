// Fill out your copyright notice in the Description page of Project Settings.


#include "Money.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/PlayerCharacter.h"

// Overridden activate function
void AMoney::Activate()
{
	Super::Activate();

	// We give the player the money
	Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(this, APlayerCharacter::StaticClass()))->UpdateMoney(Value);

	// And then destroy the actor
	this->Destroy();
}

// Called every frame
void AMoney::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}