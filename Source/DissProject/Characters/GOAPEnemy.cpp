// Fill out your copyright notice in the Description page of Project Settings.

#include "GOAPEnemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Pickups/SpawnerBase.h"
#include "Sound/SoundCue.h"
#include "Laser.h"
#include "GameFramework/CharacterMovementComponent.h"

// Gloabl constants
static const float ANIMATION_MESH_OFFSET = -85.f; // The offset for the character mesh to be on the ground
static const float GUN_OFFSET = 20.f; // The offset for the gun to be in front

// Sets default values
AGOAPEnemy::AGOAPEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the meshes components and add them to root
	RangedMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Ranged Mesh"));
	RangedMeshComponent->SetupAttachment(this->RootComponent);
	RangedMeshComponent->AddLocalOffset(FVector(GUN_OFFSET, 0.f, 0.f));
	RangedMeshComponent->AddLocalRotation(FRotator(0.f, -90.f, 0.f));

	// Rotate the mesh to face forward
	GetMesh()->AddLocalRotation(FRotator(0.f, -90.f, 0.f));
	
	// Create the inventory and action base
	CreateInventory();
	CreateActionBase();

	// Set up the AI controller
	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Set the mesh for the enemy
	USkeletalMesh* EnemyMesh = nullptr;
	while (!EnemyMesh)
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/PolygonScifi/Meshes/Characters/SK_Character_Alien_Male_02.SK_Character_Alien_Male_02"));
		EnemyMesh = MeshAsset.Object;
	}
	GetMesh()->SetSkeletalMesh(EnemyMesh);
	GetMesh()->AddLocalOffset(FVector(0.f, 0.f, ANIMATION_MESH_OFFSET));
}

// Called when the game starts or when spawned
void AGOAPEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Make sure we have the controller instance
	AIController = Cast<AAIController>(GetController());
	
	// Get all the weapon spawners
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnerBase::StaticClass(), WeaponSpawners);

	// Get the reference to the player
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(this, APlayerCharacter::StaticClass()));

	// Formulate a plan
	Plan = GetPlan();
	TakeAction();
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
	AttackSoundComponent->bAutoActivate = false;
	static ConstructorHelpers::FObjectFinder<USoundCue> RangedSoundAsset(TEXT("/Game/Assets/Audio/SC_Laser.SC_Laser"));
	RangedSound = RangedSoundAsset.Object;
	AttackSoundComponent->SetSound(RangedSound);

	// Equip the gun
	Equip(Gun);
}

// Called to create the actionbase
void AGOAPEnemy::CreateActionBase()
{
	// Base GOAP states
	FGOAPState PlayerDead = FGOAPState(EVariableType::FLOAT, EStateCase::HEALTH, "Player", false, 0.f);
	FGOAPState CanSeePlayer = FGOAPState(EVariableType::VECTOR, EStateCase::SIGHT, "Player");
	FGOAPState HasAmmo = FGOAPState(EVariableType::BOOLEAN, EStateCase::AMMO, "Ammo", true);
	FGOAPState BeAlive = FGOAPState(EVariableType::FLOAT, EStateCase::HEALTH, GetName(), false, 101.f);

	// Building action to attack player
	TArray<FGOAPState> AttackPlayerPreconditions;
	AttackPlayerPreconditions.Emplace(CanSeePlayer);
	AttackPlayerPreconditions.Emplace(HasAmmo);
	TArray<FGOAPState> AttackPlayerEffects;
	AttackPlayerEffects.Init(PlayerDead, 1);
	FAction AttackPlayer = FAction(AttackPlayerEffects, EStateCase::HEALTH, "Attack", AttackPlayerPreconditions);

	// Building action to go to player
	TArray<FGOAPState> GoToPlayerPreconditions;
	GoToPlayerPreconditions.Init(BeAlive, 1);
	TArray<FGOAPState> GoToPlayerEffects;
	GoToPlayerEffects.Init(CanSeePlayer, 1);
	FAction GoToPlayer = FAction(GoToPlayerEffects, EStateCase::LOCATION, "MoveToLocation", GoToPlayerPreconditions);

	// Building action to go to ammo
	TArray<FGOAPState> GoToAmmoPreconditions;
	GoToAmmoPreconditions.Init(BeAlive, 1);
	TArray<FGOAPState> GoToAmmoEffects;
	GoToAmmoEffects.Init(HasAmmo, 1);
	FAction GoToAmmo = FAction(GoToAmmoEffects, EStateCase::LOCATION, "MoveToLocation", GoToAmmoPreconditions);

	// Add them all to the action base
	Actions.Emplace(AttackPlayer);
	Actions.Emplace(GoToPlayer);
	Actions.Emplace(GoToAmmo);
}

// Called every frame
void AGOAPEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If we are mid-move, check location against goal
	if (IsMoving)
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::FromInt(1));
		// If we are at the goal, stop moving and take next action
		if (IsMovingToPlayer && UKismetMathLibrary::Vector_Distance(GetActorLocation(), Player->GetActorLocation()) <= CurrentWeapon.Range * 0.6f)
		{
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::FromInt(2));
			IsMoving = false;
			IsMovingToPlayer = false;
			AIController->StopMovement();
			TakeAction();
		}else if (!IsMovingToPlayer && UKismetMathLibrary::Vector_Distance(GetActorLocation(), MovingToLocation) <= LocationErrorMargin)
		{
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::FromInt(3));
			IsMoving = false;
			AIController->StopMovement();
			TakeAction();
		}
	}
	else if (!IsMoving && !Attacking) { TakeAction(); GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::FromInt(4)); }
	else if (AttackTimer <= 0.f && Attacking) { SpawnLaser(); GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, FString::FromInt(5)); }
	
	if (AttackTimer > 0.f) AttackTimer -= DeltaTime;
	
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, "AttackTimer: " + FString::SanitizeFloat(AttackTimer));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, "Moving: " + FString::FromInt(int32(IsMoving)) + " - Attacking: " + FString::FromInt(int32(Attacking)));

	if (Plan.IsValidIndex(0))
	{
		FString PlanOfAction;
		for (auto& act : Plan)
		{
			PlanOfAction += act.Effects[0].Subject;
		}
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, PlanOfAction);
	}
	else GetPlan();
}

// Called to create a plan of actions
TArray<FAction> AGOAPEnemy::GetPlan()
{
	// Use A* algorithm to find the shortest path
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
			Steps++;
		}
		if (Steps >= 5) this->Destroy();
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

	// These are the heuristic of the A* search algorithm
	FAction ActionWithMostMetPreconditions = Actions[0];
	int NumOfUnmetPreconditions = 999;

	// Loop through all known actions...
	for (auto& Action : Actions)
	{
		// ...and see if any of the effects fulfil the precondition passed in
		for (auto& Effect : Action.Effects)
		{
			if (Effect == Precondition)
			{
				// Get the amount of unmet preconditions for this action, and compare it to the current value
				int32 UnmetPreconditions = GetNumberOfUnmetPreconditions(Action);
				if (UnmetPreconditions < NumOfUnmetPreconditions)
				{
					// If it's lower, we want this action, as it will be quicker
					ActionWithMostMetPreconditions = Action;
					NumOfUnmetPreconditions = UnmetPreconditions;
				}
				break;
			}
		}	
	}

	// If there was no plan found, we return NULL, passing all the way back up until to increment the search depth
	TArray<FAction> TestPlan = CurrentPlan;
	TestPlan.Emplace(ActionWithMostMetPreconditions);
	for (auto& ActionPrecondition : ActionWithMostMetPreconditions.Preconditions)
	{
		TestPlan = Formulate(Steps - 1, ActionPrecondition, TestPlan);
		if (!TestPlan.IsValidIndex(0))
		{
			TArray<FAction> nullArray;
			return nullArray;
		}
	}

	// If we haven't returned a null array by now, the plan must be valid
	return TestPlan;
}
int32 AGOAPEnemy::GetNumberOfUnmetPreconditions(FAction Action)
{
	int32 UnmetPreconditions = 0;
	for (auto& Precondition : Action.Preconditions)
	{
		if (!ValidatePrecondition(Precondition)) UnmetPreconditions++;
	}
	return UnmetPreconditions;
}

// Called to ensure the current plan is valid
bool AGOAPEnemy::ValidatePlan(TArray<FAction> TestPlan)
{
	if (!TestPlan.IsValidIndex(0)) return false;
	
	// And an array of States that are so far true in the plan formulation, start it off with just saying the enemy is alive
	TArray<FGOAPState> TrueStates;
	FGOAPState BeAlive = FGOAPState(EVariableType::FLOAT, EStateCase::HEALTH, GetName(), false, 101.f);
	TrueStates.Init(BeAlive, 1);
	
	// Starting from the first action in the plan, loop through the plan
	for (int32 PlanIndex = TestPlan.Num() - 1; PlanIndex >= 0; --PlanIndex)
	{
		// Check all the preconditions of the action is in the true states array
		for (auto& Precondition : TestPlan[PlanIndex].Preconditions)
		{
			bool PreconditionMet = false;
			for (auto& TrueState : TrueStates)
			{
				if (TrueState == Precondition)
				{
					PreconditionMet = true;
					break;
				}
			}
			// If it's not, then the plan isn't valid
			if (!PreconditionMet && !ValidatePrecondition(Precondition)) return false;
		}
		// If the preconditions are met, add the effects of this action to the true states of the array
		for (auto& Effect : TestPlan[PlanIndex].Effects)
		{
			TrueStates.Emplace(Effect);
		}
	}

	// As long as we haven't returned false by now, the plan is valid, so return true
	return true;
}

// Called when an enemy wants to initiate an action, returns whether the action is valid
bool AGOAPEnemy::TakeAction()
{
	// Make sure the new plan is valid
	bool PlanValid = ValidatePlan(Plan);
	while (!PlanValid) Plan = GetPlan();

	// Remove the next action from the plan, and prepare to facilitate
	FAction Action = Plan.Pop(true);
	
	// Check if the preconditions are met
	for (int32 PreconChecker = 0; PreconChecker < Action.Preconditions.Num(); ++PreconChecker)
	{
		if (!ValidatePrecondition(Action.Preconditions[PreconChecker])) return false;
	}
	// We then get the specifics of the action
	CalculateAction(Action);

	// If we haven't returned false, we are good to go
	return true;
}
void AGOAPEnemy::CalculateAction(FAction Action)
{
	// Get the action to take, and take the action
	if (Action.Action == "Attack") Attack();
	else if (Action.Action == "MoveToLocation") MoveToLocation(Action);
}
void AGOAPEnemy::Attack()
{
	// If we have ammo, attack
	if (CurrentWeapon.Ammo > 0)
	{
		if (!Attacking) Attacking = true;
	}
	else // If we don't have ammo, flick through the inventory to see if there is any gun with ammo
	{
		Attacking = false;

		// First we save the current weapon
		Inventory->Inventory[CurrentInventoryItem].Ammo = CurrentWeapon.Ammo;

		// We save the index we started on
		int32 StartingIndex = CurrentInventoryItem;

		while (CurrentWeapon.Ammo == 0 && CurrentInventoryItem != StartingIndex)
		{
			// Wrap the item around the valid indicies
			if (CurrentInventoryItem < 0) CurrentInventoryItem = Inventory->Inventory.Num() - 1;
			if (CurrentInventoryItem == Inventory->Inventory.Num()) CurrentInventoryItem = 0;

			// If we find a gun that has ammo, equip it
			if (Inventory->Inventory[CurrentInventoryItem].Ammo > 0 && Inventory->Inventory[CurrentInventoryItem].Type == EWeaponType::RANGED)
			{
				CurrentWeapon = Inventory->Inventory[CurrentInventoryItem];
				break;
			}
			++CurrentInventoryItem;
		}
		if (CurrentWeapon.Ammo > 0)
		{
			Equip(Inventory->Inventory[CurrentInventoryItem]);
			Attack();
		}
	}
}
void AGOAPEnemy::SpawnLaser()
{
	// We make sure we are facing the player
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Player->GetActorLocation()));

	// These are the parameters for the laser
	FActorSpawnParameters SpawnParams;
	FVector SpawnLoc = GetActorLocation() + GetActorForwardVector() * 100;
	FRotator SpawnRot = FRotator(0, 0, 0);
	ALaser* Laser;

	// Spawn a laser and assign its attributes
	Laser = GetWorld()->SpawnActor<ALaser>(ALaser::StaticClass(), SpawnLoc, SpawnRot, SpawnParams);
	Laser->SetupLaser(CurrentWeapon.Range, CurrentWeapon.Damage, GetActorForwardVector(), 100.f);
	AttackSoundComponent->Play();
	AttackTimer = CurrentWeapon.RateOfFire;
	--CurrentWeapon.Ammo;

	// And turn off the attack
	Attacking = false;
}
void AGOAPEnemy::MoveToLocation(FAction Action)
{
	// Set to moving and attacking to false
	IsMoving = true;
	Attacking = false;

	// If the goal is to get ammo...
	if (Action.Effects[0].StateCase == EStateCase::AMMO)
	{
		// ...we find the closest spawner...
		AActor* ClosestSpawner = nullptr;
		for (auto& Spawner : WeaponSpawners)
		{
			if (ClosestSpawner)
			{
				float ThisSpawnerDistance = UKismetMathLibrary::Vector_Distance(GetActorLocation(), Spawner->GetActorLocation());
				if (UKismetMathLibrary::Vector_Distance(GetActorLocation(), ClosestSpawner->GetActorLocation()) < ThisSpawnerDistance) ClosestSpawner = Spawner;
			}
			else ClosestSpawner = Spawner;
		}
		// ...and set the MoveTo goal to its location
		MovingToLocation = ClosestSpawner->GetActorLocation();
		AIController->MoveToActor(ClosestSpawner);
	}
	// If the goal is instead to see the player...
	else
	{
		// ...we move to the player's location
		IsMovingToPlayer = true;
		MovingToLocation = Player->GetActorLocation();
		AIController->MoveToActor(Player);
	}
}

// Called to equip a weapon
void AGOAPEnemy::Equip(FWeaponDetails Weapon)
{
	CurrentWeapon = Weapon;
	RangedMeshComponent->SetSkeletalMesh(Weapon.RangedMesh);
}

// Called to validate an action's precondition is met
bool AGOAPEnemy::ValidatePrecondition(FGOAPState Precondition)
{
	// Here we filter the precondition through a switch case to validate that whatever variable needs to be true is true
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
		if (Precondition.StateCase == EStateCase::LOCATION) return CheckLocation(Precondition.VectorValue);
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
	CollisionParameters.AddIgnoredActors(WeaponSpawners);
	
	if (Actor == "")
	{
		// Here we do a ray trace to see if the location is in line of sight
		if (GetWorld()->LineTraceSingleByChannel(HitActor, RayStart, LookAtLocation, ECC_Visibility, CollisionParameters))
		{
			// We check to see if the hit location is within an acceptable distance of the target to allow for volume boundries
			if (UKismetMathLibrary::Vector_Distance(HitActor.Location, LookAtLocation) <= LocationErrorMargin) return true;
		}
	}
	else if (Actor == "Player")
	{
		FVector PlayerLocation = Player->GetActorLocation();
		// Here we do a ray trace to see if the player is in line of sight
		if (GetWorld()->LineTraceSingleByChannel(HitActor, RayStart, PlayerLocation, ECC_Visibility, CollisionParameters))
		{
			// We check to see if the hit location is within an acceptable distance of the target to allow for volume boundries
			if (HitActor.Actor == Player && UKismetMathLibrary::Vector_Distance(HitActor.Location, GetActorLocation()) < CurrentWeapon.Range * 0.7f) return true;
			Attacking = false;
		}
	}

	// Return false if not already returned
	return false;
}
// Checks if actor is at location
bool AGOAPEnemy::CheckLocation(FVector IsAtLocation)
{
	// We make sure the 
	return UKismetMathLibrary::Vector_Distance(GetActorLocation(), IsAtLocation) <= LocationErrorMargin;
}
// Checks the health of the given actor, returns true if below or equal to given float
bool AGOAPEnemy::CheckHealth(float InHealth, FString InSubject)
{
	// Check the subject's class, and check the health accordingly
	if (InSubject.Contains("Player")) return (Player->Health <= InHealth);
	else return (Health <= InHealth);

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