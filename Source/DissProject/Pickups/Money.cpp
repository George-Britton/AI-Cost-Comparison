// Fill out your copyright notice in the Description page of Project Settings.


#include "Money.h"
#include "../Characters/PlayerCharacter.h"

// Overridden activate function
void AMoney::Activate()
{
	Super::Activate();

	if (OverlappedActor->GetName().Contains("Player"))
	{
		// We give the player the money
		Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(this, APlayerCharacter::StaticClass()))->PickUp(nullptr, Value);

		// And then destroy the actor
		this->Destroy();
	}
		
}
