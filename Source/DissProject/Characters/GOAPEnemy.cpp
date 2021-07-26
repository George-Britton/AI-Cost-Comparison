// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPEnemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "PlayerCharacter.h"

// Sets default values
AGOAPEnemy::AGOAPEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the meshes components and add them to root
	MeleeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Melee Mesh"));
	RangedMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Ranged Mesh"));
	MeleeMeshComponent->SetupAttachment(this->RootComponent);
	RangedMeshComponent->SetupAttachment(this->RootComponent);

	// Create the inventory object
	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));

	// Create a knife weapon and add it to the inventory
	static ConstructorHelpers::FObjectFinder<UStaticMesh> KnifeAsset(TEXT("/Game/PolygonScifi/Meshes/Weapons/Accessories/SM_Wep_Knife_01.SM_Wep_Knife_01"));
	FWeaponDetails Knife = FWeaponDetails::FWeaponDetails("Knife", KnifeAsset.Object, nullptr, 10, 0.1f, 0.f, 0.f, EWeaponType::MELEE);
	Inventory->AddItem(Knife);
	CurrentWeapon = Inventory->Inventory[0];

	// Create a gun and add it to the inventory
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunAsset(TEXT("/Game/PolygonScifi/Meshes/Weapons/SK_Wep_Rifle_Plasma_01.SK_Wep_Rifle_Plasma_01"));
	FWeaponDetails Gun = FWeaponDetails::FWeaponDetails("Plasma Rifle", nullptr, GunAsset.Object, 20.f, 0.33f, 4000.f, 36, EWeaponType::RANGED, EWeaponSpeed::ONESHOT);
	Inventory->AddItem(Gun);

	// The audio of the attacks
	AttackSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Attack Sound Component"));
	AttackSoundComponent->SetupAttachment(this->RootComponent);
	AttackSoundComponent->bAllowSpatialization = true;
	static ConstructorHelpers::FObjectFinder<USoundCue> RangedSoundAsset(TEXT("/Game/Assets/Audio/SC_Laser.SC_Laser"));
	RangedSound = RangedSoundAsset.Object;
	static ConstructorHelpers::FObjectFinder<USoundCue> MeleeSoundAsset(TEXT("/Game/Assets/Audio/SC_Swipe.SC_Swipe"));
	RangedSound = MeleeSoundAsset.Object;

	// Set up the AI controller
	AIControllerClass = AGOAPAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AGOAPEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Make sure we have the controller instance
	GOAPController = Cast<AGOAPAIController>(GetController());
}

// Called every frame
void AGOAPEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to create a plan of actions
TArray<FAction> AGOAPEnemy::Formulate()
{
	TArray<FAction> outputAction;
	outputAction.Init(FAction::FAction(), 1);
	return outputAction;
}

// Called to ensure the current plan is valid
bool AGOAPEnemy::ValidatePlan(TArray<FAction> TestPlan)
{
	return false;
}

// Called when an enemy wants to initiate an action, returns whether the action is valid
bool AGOAPEnemy::TakeAction(FAction Action)
{
	// Check if the preconditions are met
	for (int32 PreconChecker = 0; PreconChecker < Action.Preconditions.Num(); ++PreconChecker)
	{
		if (!ValidatePrecondition(Action.Preconditions[PreconChecker])) return false;
	}

	return true;
}

// Called to validate an action's precondition is met
bool AGOAPEnemy::ValidatePrecondition(FGOAPState Precondition)
{
	switch (Precondition.VariableType)
	{
	case EVariableType::BOOLEAN:
		if (Precondition.StateCase == EStateCase::AMMO) return CheckAmmo();
	case EVariableType::FLOAT:
		if (Precondition.StateCase == EStateCase::HEALTH) return CheckHealth(Precondition.FloatValue, Precondition.Subject);
	case EVariableType::INTEGER:
		
	case EVariableType::VECTOR:
		if (Precondition.StateCase == EStateCase::SIGHT) return CheckSight(Precondition.VectorValue);
		else if (Precondition.StateCase == EStateCase::LOCATION) return CheckLocation(Precondition.VectorValue);
	default: return true;
	}
}

// Called to query the world state
// Checks line of sight
bool AGOAPEnemy::CheckSight(FVector LookAtLocation)
{
	// These are the parameters for the ray trace
	FHitResult HitActor;
	FVector RayStart = GetActorLocation();
	FCollisionQueryParams CollisionParameters;
	CollisionParameters.AddIgnoredActor(this);

	// Here we do a ray trace to see if the location is in line of sight
	if (GetWorld()->LineTraceSingleByChannel(HitActor, RayStart, LookAtLocation, ECC_Visibility, CollisionParameters))
	{
		// We check to see if the hit location is within an acceptable distance of the target to allow for volume boundries
		if (UKismetMathLibrary::Vector_Distance(HitActor.Location, LookAtLocation) <= LocationErrorMargin) return true;
	}

	// return false if not already returned
	return false;
}
// Checks if actor is at location
bool AGOAPEnemy::CheckLocation(FVector IsAtLocation)
{
	return UKismetMathLibrary::Vector_Distance(GetActorLocation(), IsAtLocation) <= LocationErrorMargin;
}
// Checks the health of the given actor, returns true if below or equal to given float
bool AGOAPEnemy::CheckHealth(float InHealth, AActor* InSubject)
{
	// Check the subject's class, and check the health accordingly
	if (InSubject->GetClass() == APlayerCharacter::StaticClass()) return Cast<APlayerCharacter>(InSubject)->Health <= InHealth;
	else if (InSubject->GetClass() == AGOAPEnemy::StaticClass()) return Cast<AGOAPEnemy>(InSubject)->Health <= InHealth;

	// return true as a default in case of null pointer
	return true;
}
// Checks the ammo of the enemy, returns true if has ammo
bool AGOAPEnemy::CheckAmmo()
{
	// Loop through the inventory and check if anything has ammo
	for (int32 InventoryIndex = 0; InventoryIndex < Inventory->Inventory.Num(); ++InventoryIndex)
	{
		FWeaponDetails CheckingWeapon = Inventory->Inventory[InventoryIndex];
		if (CheckingWeapon.Type == EWeaponType::RANGED && CheckingWeapon.Ammo > 0) return true;
	}

	// Return false if there was no ranged ammo found
	return false;
}