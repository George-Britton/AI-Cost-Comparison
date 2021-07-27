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
	RangedMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Ranged Mesh"));
	RangedMeshComponent->SetupAttachment(this->RootComponent);

	// Create the inventory
	CreateInventory();

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

// Called to create the inventory
void AGOAPEnemy::CreateInventory()
{
	// Create the inventory object
	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));
	
	// Create a gun and add it to the inventory
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunAsset(TEXT("/Game/PolygonScifi/Meshes/Weapons/SK_Wep_Rifle_Plasma_01.SK_Wep_Rifle_Plasma_01"));
	FWeaponDetails Gun = FWeaponDetails::FWeaponDetails("Plasma Rifle", nullptr, GunAsset.Object, 20.f, 0.33f, 4000.f, 36, EWeaponType::RANGED, EWeaponSpeed::ONESHOT);
	Inventory->AddItem(Gun);
	CurrentWeapon = Inventory->Inventory[0];

	// The audio of the attacks
	AttackSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Attack Sound Component"));
	AttackSoundComponent->SetupAttachment(this->RootComponent);
	AttackSoundComponent->bAllowSpatialization = true;
	static ConstructorHelpers::FObjectFinder<USoundCue> RangedSoundAsset(TEXT("/Game/Assets/Audio/SC_Laser.SC_Laser"));
	RangedSound = RangedSoundAsset.Object;
}

// Called to create the actionbase
void AGOAPEnemy::CreateActionBase()
{
	// Base GOAP states
	FGOAPState PlayerDead = FGOAPState(EVariableType::FLOAT, EStateCase::HEALTH, "Player", false, 0.f);
	FGOAPState CanSeePlayer = FGOAPState(EVariableType::VECTOR, EStateCase::SIGHT, "Player");
	FGOAPState HasAmmo = FGOAPState(EVariableType::BOOLEAN, EStateCase::AMMO, "", true);
	FGOAPState BeAlive = FGOAPState(EVariableType::FLOAT, EStateCase::HEALTH, GetName(), false, 100.f);

	// Building action to attack player
	TArray<FGOAPState> AttackPlayerPreconditions;
	AttackPlayerPreconditions.Emplace(CanSeePlayer);
	AttackPlayerPreconditions.Emplace(HasAmmo);
	TArray<FGOAPState> AttackPlayerEffects;
	AttackPlayerEffects.Init(PlayerDead, 1);
	FAction AttackPlayer = FAction(AttackPlayerEffects, EStateCase::HEALTH, "Attack", AttackPlayerPreconditions);

	// Building action to go to player
	TArray<FGOAPState> GoToPlayerPreconditions;
	GoToPlayerPreconditions.Init(HasAmmo, 1);
	TArray<FGOAPState> GoToPlayerEffects;
	GoToPlayerEffects.Init(CanSeePlayer, 1);
	FAction GoToPlayer = FAction(GoToPlayerEffects, EStateCase::LOCATION, "MoveToLocation", GoToPlayerPreconditions);

	// Building action to go to ammo
	TArray<FGOAPState> GoToAmmoPreconditions;
	GoToAmmoPreconditions.Init(HasAmmo, 1);
	TArray<FGOAPState> GoToAmmoEffects;
	GoToAmmoEffects.Init(BeAlive, 1);
	FAction GoToAmmo = FAction(GoToAmmoEffects, EStateCase::LOCATION, "MoveToLocation", GoToAmmoPreconditions);
}

// Called every frame
void AGOAPEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to create a plan of actions
TArray<FAction> AGOAPEnemy::GetPlan()
{
	// Use IDDFS algorithm to find the shortest path
	bool GotPlan = false; // The flag for having a valid plan
	int32 Steps = 0; // The number of actions it will take to reach the goal
	FGOAPState Goal = FGOAPState(EVariableType::FLOAT, EStateCase::HEALTH, "Player", false, 0.f); // The goal state of having the player dead
	TArray<FAction> PlanInWorks; // The finally output plan

	// While we don't have a plan, loop with increasing plan depth
	while (!GotPlan)
	{
		// Get a plan from the Formulate function
		PlanInWorks = Formulate(Steps, Goal, PlanInWorks);

		// If the plan is NULL or invalid, clear the plan, and go again but deeper
		if (PlanInWorks.IsValidIndex(0)) GotPlan = ValidatePlan(PlanInWorks);
		if (!GotPlan)
		{
			PlanInWorks.Empty();
			++Steps;
		}
	}

	// Return the validated plan
	return PlanInWorks;
}
TArray<FAction> AGOAPEnemy::Formulate(int32 Steps, FGOAPState Precondition, TArray<FAction> CurrentPlan)
{
	// if the precondition is already met, then the plan is valid
	if (ValidatePrecondition(Precondition)) return CurrentPlan;
	// If the depth reached is 0, then there is no valid plan for the initial depth (*Inception voice* "We need to go deeper")
	if (Steps == 0)
	{
		TArray<FAction> NullArray;
		return NullArray;
	}

	// Make a copy of the plan to mess around with and add possible actions to
	TArray<FAction> NewPlan = CurrentPlan;

	// Loop through all known actions...
	for (auto& Action : Actions)
	{
		// ...and see if any of the effects fulfil the precondition passed in
		for (auto& Effect : Action.Effects)
		{
			if (Effect != Precondition) continue; // If not, continue searching

			// If it does, add the action to the plan, and set a counter for unmet preconditions for the new action
			NewPlan.Add(Action);
			int32 UnmetPreconditions = 0;

			// Loop through the action's preconditions, and see how many of them are unmet
			for (auto& Preconditions : Action.Preconditions)
			{
				TArray<FAction> TestPlan = Formulate(Steps - 1, Precondition, NewPlan);
				if (!TestPlan.IsValidIndex(0)) ++UnmetPreconditions;
			}
			// If there are no unmet preconditions, then we have the final plan
			if (UnmetPreconditions == 0) return NewPlan;

			// If there are unmet preconditions, then we know that this action isn't the end, so go to the next
			NewPlan.RemoveAt(NewPlan.Num() - 1);
		}
	}

	// If there was no plan found, we return NULL, passing all the way back up until to increment the search depth
	TArray<FAction> NullArray;
	return NullArray;
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
		break;
	case EVariableType::VECTOR:
		if (Precondition.StateCase == EStateCase::SIGHT) return CheckSight(Precondition.VectorValue, Precondition.Subject);
		else if (Precondition.StateCase == EStateCase::LOCATION) return CheckLocation(Precondition.VectorValue);
	default: return true;
	}

	return true;
}

// Called to query the world state
// Checks line of sight
bool AGOAPEnemy::CheckSight(FVector LookAtLocation, FString Actor)
{
	// These are the parameters for the ray trace
	FHitResult HitActor;
	FVector RayStart = GetActorLocation();
	FCollisionQueryParams CollisionParameters;
	CollisionParameters.AddIgnoredActor(this);

	if (Actor == "")
	{
		// Here we do a ray trace to see if the location is in line of sight
		if (GetWorld()->LineTraceSingleByChannel(HitActor, RayStart, LookAtLocation, ECC_Visibility, CollisionParameters))
		{
			// We check to see if the hit location is within an acceptable distance of the target to allow for volume boundries
			if (UKismetMathLibrary::Vector_Distance(HitActor.Location, LookAtLocation) <= LocationErrorMargin) return true;
		}
	}
	else
	{
		if (Actor.Contains("Player"))
		{
			FVector PlayerLocation = UGameplayStatics::GetActorOfClass(this, APlayerCharacter::StaticClass())->GetActorLocation();
			// Here we do a ray trace to see if the player is in line of sight
			if (GetWorld()->LineTraceSingleByChannel(HitActor, RayStart, PlayerLocation, ECC_Visibility, CollisionParameters))
			{
				// We check to see if the hit location is within an acceptable distance of the target to allow for volume boundries
				if (HitActor.Actor->GetClass() == APlayerCharacter::StaticClass()) return true;
			}
		}
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
bool AGOAPEnemy::CheckHealth(float InHealth, FString InSubject)
{
	// Check the subject's class, and check the health accordingly
	if (InSubject.Contains("Player")) return (Cast<APlayerCharacter>(Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(this, APlayerCharacter::StaticClass())))->Health <= InHealth);
	else if (InSubject == GetName()) return (Health <= InHealth);

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