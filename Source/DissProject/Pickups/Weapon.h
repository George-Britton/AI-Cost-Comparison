// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "Weapon.generated.h"

UENUM()
enum class EWeaponType : uint8
{
	MELEE UMETA(DisplayName="Melee"),
	RANGED UMETA(DisplayName="Ranged"),
	MAX
};

UENUM()
enum class EWeaponSpeed : uint8
{
	ONESHOT UMETA(DisplayName="One-Shot"),
	AUTOMATIC UMETA(DisplayName="Automatic"),
	MAX
};

// Struct with the boss' attack details
USTRUCT(BlueprintType)
struct FWeaponDetails
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMesh* MeleeMesh = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMesh* RangedMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintreadWrite)
		float Damage;

	UPROPERTY(EditAnywhere, BlueprintreadWrite)
		float RateOfFire;

	UPROPERTY(EditAnywhere, BlueprintreadWrite)
		float Range;

	UPROPERTY(EditAnywhere, BlueprintreadWrite)
		uint8 Ammo;

	UPROPERTY(EditAnywhere, BlueprintreadWrite)
		EWeaponType Type;
	
	UPROPERTY(EditAnywhere, BlueprintreadWrite)
		EWeaponSpeed Speed;

	FWeaponDetails(FName InName = "None", UStaticMesh* InMeleeMesh = nullptr, USkeletalMesh * InRangedMesh = nullptr, float InDamage = 10.f, float InRateOfFire = 0.33f, float InRange = 4000.f, uint8 InAmmo = 18, EWeaponType InType = EWeaponType::RANGED, EWeaponSpeed InSpeed = EWeaponSpeed::ONESHOT)
	{
		Name = InName;
		MeleeMesh = InMeleeMesh;
		RangedMesh = InRangedMesh;
		Damage = InDamage;
		RateOfFire = InRateOfFire;
		Range = InRange;
		Ammo = InAmmo;
		Type = InType;
		Speed = InSpeed;
	}

	FWeaponDetails(FWeaponDetails* InDetails)
	{
		Name = InDetails->Name;
		MeleeMesh = InDetails->MeleeMesh;
		RangedMesh = InDetails->RangedMesh;
		Damage = InDetails->Damage;
		RateOfFire = InDetails->RateOfFire;
		Range = InDetails->Range;
		Ammo = InDetails->Ammo;
		Type = InDetails->Type;
		Speed = InDetails->Speed;
	}
};

/**
 * 
 */
UCLASS()
class DISSPROJECT_API AWeapon : public APickupBase
{
	GENERATED_BODY()

public:
	// Default constructor
	AWeapon();

	// The mesh of the weapon
	UPROPERTY()
		USkeletalMeshComponent* WeaponMeshComponent = nullptr;

	// The weapon's characteristics
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FWeaponDetails WeaponDetails = FWeaponDetails::FWeaponDetails();

protected:
	// Called when a value changes
	void OnConstruction(const FTransform& Transform) override;

public:
	// Overridden activate function
	void Activate() override;
	
};
