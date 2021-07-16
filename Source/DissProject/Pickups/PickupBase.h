// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "PickupBase.generated.h"

// Event dispatcher for the overlapping of the player and the pickup
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FToggleOverlap, class APickupBase*, Pickup, bool, IsOverlapping);

UCLASS()
class DISSPROJECT_API APickupBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupBase();

	// The name of the pickup
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
		FName PickupName = "None";
	// The mesh of the pickup
	UPROPERTY(EditAnywhere, Category = "Appearance")
		UStaticMesh* PickupMesh = nullptr;
	UPROPERTY()
		UStaticMeshComponent* PickupMeshComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "Appearance")
		float SpinSpeed = 2.f;

	// The overlap box to tell the player they are within range of the pickup
	UPROPERTY(EditAnywhere, Category = "Interaction")
		float PickupSphereRadius = 150.f;
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		USphereComponent* PickupSphere = nullptr;
	UPROPERTY()
		AActor* OverlappedActor;

	// Event dispatcher for the overlaps
	UPROPERTY(BlueprintAssignable, Category = "Events")
		FToggleOverlap OnPlayerOverlap;

protected:
	// Called when a value changes
	void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:	
	// This is used to announce when the player overlaps the sphere
	UFUNCTION()
		void OnSphereOverlapFunction(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// This is used to announce when the player stops overlapping the sphere
	UFUNCTION()
		void OnSphereEndOverlapFunction(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Override to activate the pickup ability, such as a gun equip or money collect
	UFUNCTION(BlueprintCallable, Category = "Events")
		virtual void Activate();
};
