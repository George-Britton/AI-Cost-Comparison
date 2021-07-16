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

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Overridden activate function
void AWeapon::Activate()
{
	if (OverlappedActor)
	{
		if (OverlappedActor->GetClass() == APlayerCharacter::StaticClass())
		{
			Cast<APlayerCharacter>(OverlappedActor)->PickUp(&WeaponDetails, 0);
		}
		//else if (OverlappedActor->GetClass() == AEnemy::StaticClass()){}

		this->Destroy();
	}
}