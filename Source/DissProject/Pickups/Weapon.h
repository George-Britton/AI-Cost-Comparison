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

/**
 * 
 */
UCLASS()
class DISSPROJECT_API AWeapon : public APickupBase
{
	GENERATED_BODY()

public:
	// Default constructor
	AWeapon() {};

	// The mesh of the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		USkeletalMesh* WeaponMesh = nullptr;
	UPROPERTY()
		USkeletalMeshComponent* WeaponMeshComponent = nullptr;

	// The weapon's characteristics
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		EWeaponType WeaponType = EWeaponType::RANGED;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float Damage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "WeaponType == EWeaponType::RANGED"))
		float Range = 4000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "WeaponType == EWeaponType::RANGED"))
		EWeaponSpeed WeaponSpeed = EWeaponSpeed::ONESHOT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "WeaponType == EWeaponType::RANGED && WeaponSpeed == EWeaponSpeed::Automatic"))
		float RateOfFire = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "WeaponType == EWeaponType::RANGED"))
		uint8 Ammo = 0;

protected:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};
