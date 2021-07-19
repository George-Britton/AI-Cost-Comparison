// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "../Characters/PlayerCharacter.h"

// Default constructor
AWeapon::AWeapon()
{
	// Make the mesh component and add it to root
	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh Component"));
	WeaponMeshComponent->SetupAttachment(this->RootComponent);
}

// Called when a value changes
void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (WeaponDetails.RangedMesh && WeaponDetails.Type == EWeaponType::RANGED)
	{
		WeaponMeshComponent->SetSkeletalMesh(WeaponDetails.RangedMesh);
		PickupMeshComponent->SetStaticMesh(nullptr);
	}
	else if (WeaponDetails.MeleeMesh && WeaponDetails.Type == EWeaponType::MELEE)
	{
		PickupMeshComponent->SetStaticMesh(WeaponDetails.MeleeMesh);
		WeaponMeshComponent->SetSkeletalMesh(nullptr);
	}
}

// Overridden activate function
void AWeapon::Activate()
{
	Super::Activate();

	if (OverlappedActor->GetName().Contains("Player"))
	{
		// We give the player the money
		Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(this, APlayerCharacter::StaticClass()))->PickUp(&WeaponDetails, 0);

		// And then destroy the actor
		this->Destroy();
	}
}