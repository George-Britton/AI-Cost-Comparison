// Fill out your copyright notice in the Description page of Project Settings.


#include "Money.h"
#include "../Characters/PlayerCharacter.h"

// Default constructor
AMoney::AMoney()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CoinAsset(TEXT("/Game/PolygonScifi/Meshes/Props/SM_Prop_Window_01.SM_Prop_Window_01"));
	PickupMeshComponent->SetStaticMesh(CoinAsset.Object);
	PickupMeshComponent->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));
}

// Overridden activate function
void AMoney::Activate()
{
	Super::Activate();

	if (OverlappedActor->GetName().Contains("Player"))
	{
		// We give the player the money
		Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(this, APlayerCharacter::StaticClass()))->PickUp(FWeaponDetails::FWeaponDetails(), Value);
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, "Test");

		// And then destroy the actor
		this->Destroy();
	}
		
}
