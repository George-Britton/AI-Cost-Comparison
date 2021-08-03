// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Laser.generated.h"

UCLASS()
class DISSPROJECT_API ALaser : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaser();

	// The distance to travel until it stops
	UPROPERTY()
		float Range = 4000.f;

	// The speed of the laser's travel
	UPROPERTY()
		float Speed = 50.f;

	// The damage the laser does to the victim
	UPROPERTY()
		float Damage = 10.f;

	// The start point of the laser, used for measuring distance
	UPROPERTY()
		FVector StartLocation;

	// The direction of travel for the laser
	UPROPERTY()
		FVector ForwardDirection;

	// The mesh of the laser
	UPROPERTY()
		UStaticMesh* LaserMesh = nullptr;
	UPROPERTY()
		UStaticMeshComponent* LaserMeshComponent = nullptr;

	// The collision box of the laser
	UPROPERTY()
		UCapsuleComponent* CapsuleComponent = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called to set the laser's properties
	void SetupLaser(float InRange = 15000.f, float InDamage = 15.f, FVector InForwardDirection = FVector(1.f, 0.f, 0.f), float InSpeed = 10.f);

	// This is used to announce when the laser overlaps something
	UFUNCTION()
		void OnLaserOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
