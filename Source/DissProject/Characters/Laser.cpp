// Fill out your copyright notice in the Description page of Project Settings.


#include "Laser.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter.h"
#include "BTEnemy.h"

// Sets default values
ALaser::ALaser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	LaserMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Laser Mesh Component"));
	LaserMeshComponent->SetupAttachment(this->RootComponent);
	LaserMeshComponent->SetCollisionProfileName("IgnoreAll");
	LaserMeshComponent->SetGenerateOverlapEvents(true);
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	CapsuleComponent->SetupAttachment(this->RootComponent);
	CapsuleComponent->SetCollisionProfileName("OverlapAll");
	
	while (!LaserMesh)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> LaserAsset(TEXT("/Game/Assets/Meshes/SM_Laser.SM_Laser"));
		LaserMesh = LaserAsset.Object;
	}
	LaserMeshComponent->SetStaticMesh(LaserMesh);

	CapsuleComponent->InitCapsuleSize(LaserMesh->GetBounds().BoxExtent.Y, LaserMesh->GetBounds().BoxExtent.X);
	CapsuleComponent->AddLocalRotation(FRotator(0.f, 90.f, 90.f));
	CapsuleComponent->AddLocalOffset(FVector(0.f, 0.f, -(LaserMesh->GetBounds().BoxExtent.X - 5.f)));
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ALaser::OnLaserOverlap);
	CapsuleComponent->SetCollisionProfileName("Projectile");
	CapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void ALaser::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
}

// Called to set the laser's properties
void ALaser::SetupLaser(float InRange, float InDamage, FVector InForwardDirection, float InSpeed)
{
	Range = InRange;
	Damage = InDamage;
	Speed = InSpeed;

	UE_LOG(LogTemp, Warning, TEXT("Range: %s, Damage: %s, Direction: %s, Speed %s"), *FString::SanitizeFloat(InRange), *FString::SanitizeFloat(InDamage), *InForwardDirection.ToString(), *FString::SanitizeFloat(InSpeed));
	ForwardDirection = InForwardDirection;
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(StartLocation, StartLocation + ForwardDirection));
}

// This is used to announce when the laser overlaps something
void ALaser::OnLaserOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->GetName().Contains("Player") && OtherActor != this && !OtherActor->GetName().Contains("WeaponSpawner") && (!OtherActor->GetName().Contains("Enemy") || OtherActor->GetName().Contains("EnemySpawner")))
	{
		this->Destroy();
	}
	else if (OtherActor->GetName().Contains("Player"))
	{
		Cast<APlayerCharacter>(OtherActor)->RecieveAttack(Damage);
		this->Destroy();
	}
	else if (OtherActor->GetName().Contains("Enemy") && !OtherActor->GetName().Contains("EnemySpawner"))
	{
		Cast<ABTEnemy>(OtherActor)->RecieveAttack(Damage);
		this->Destroy();
	}
}

// Called every frame
void ALaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// move the laser
	this->AddActorWorldOffset(ForwardDirection * Speed, true);

	// if the laser has hit max distance, destroy it
	if (UKismetMathLibrary::Vector_Distance(StartLocation, GetActorLocation()) >= Range)
	{
		this->Destroy();
	}

}