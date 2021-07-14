// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupBase.h"

// Sets default values
APickupBase::APickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Here we initialise the basic components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	PickupMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh Component"));
	PickupMeshComponent->SetupAttachment(this->RootComponent);
	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Pickup Sphere"));
	PickupSphere->SetupAttachment(this->RootComponent);
}

// Called when a value changes
void APickupBase::OnConstruction(const FTransform& Transform)
{
	// We first set the mesh if it exists
	if (PickupMesh) PickupMeshComponent->SetStaticMesh(PickupMesh);

	// We then update the sphere's radius
	PickupSphere->SetSphereRadius(PickupSphereRadius);
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Here we assign the overlap sphere to register the player overlapping
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::OnSphereOverlapFunction);
	PickupSphere->OnComponentEndOverlap.AddDynamic(this, &APickupBase::OnSphereEndOverlapFunction);
}

// Called every frame
void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Spin the pickup
	PickupMeshComponent->AddLocalRotation(FRotator(0.f, SpinSpeed, 0.f));
}

// This is used to announce when the player overlaps the sphere
void APickupBase::OnSphereOverlapFunction(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetName().Contains("Player"))
	{
		OnPlayerOverlap.Broadcast(this, true);
	}
}

// This is used to announce when the player stops overlapping the sphere
void APickupBase::OnSphereEndOverlapFunction(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetName().Contains("Player"))
	{
		OnPlayerOverlap.Broadcast(this, false);
	}
}

// Override to activate the pickup ability, such as a gun equip or money collect
void APickupBase::Activate()
{
	OnPlayerOverlap.Broadcast(this, false);
}