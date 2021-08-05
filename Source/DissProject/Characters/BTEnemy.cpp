// Fill out your copyright notice in the Description page of Project Settings.


#include "BTEnemy.h"
#include "../Pickups/SpawnerBase.h"
#include "Sound/SoundCue.h"
#include "Laser.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Global constants
static const float ANIMATION_MESH_OFFSET = -85.f; // The offset for the character mesh to be on the ground
static const float GUN_OFFSET = 20.f; // The offset for the gun to be in front

// Sets default values
ABTEnemy::ABTEnemy()
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
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// Create the inventory
	CreateInventory();

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
void ABTEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Make sure we have the controller instance
	AIController = Cast<AAIController>(GetController());

	// Get all the weapon spawners
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnerBase::StaticClass(), WeaponSpawners);

	// Get the reference to the player
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(this, APlayerCharacter::StaticClass()));
}

// Called to create the inventory
void ABTEnemy::CreateInventory()
{
	// Create the inventory object
	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));

	// Create a gun and add it to the inventory
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> GunAsset(TEXT("/Game/PolygonScifi/Meshes/Weapons/SK_Wep_Rifle_Plasma_01.SK_Wep_Rifle_Plasma_01"));
	FWeaponDetails Gun = FWeaponDetails::FWeaponDetails("Plasma Rifle", nullptr, GunAsset.Object, 20.f, 0.33f, 4000.f, 36, EWeaponType::RANGED, EWeaponSpeed::ONESHOT);
	Inventory->AddItem(Gun);
	CurrentInventoryItem = 0;
	// Equip the gun
	Equip(Inventory->Inventory[0]);

	// The audio of the attacks
	AttackSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Attack Sound Component"));
	AttackSoundComponent->SetupAttachment(this->RootComponent);
	AttackSoundComponent->bAllowSpatialization = true;
	AttackSoundComponent->bAutoActivate = false;
	static ConstructorHelpers::FObjectFinder<USoundCue> RangedSoundAsset(TEXT("/Game/Assets/Audio/SC_Laser.SC_Laser"));
	RangedSound = RangedSoundAsset.Object;
	AttackSoundComponent->SetSound(RangedSound);
}

// Called every frame
void ABTEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Reduce the attack timer if necessary
	if (AttackTimer > 0.f) AttackTimer -= DeltaTime;
}

// Called to equip a weapon
void ABTEnemy::Equip(FWeaponDetails Weapon)
{
	CurrentWeapon = Weapon;
	RangedMeshComponent->SetSkeletalMesh(CurrentWeapon.RangedMesh);
}
void ABTEnemy::PickupWeapon(FWeaponDetails WeaponDetails)
{
	Inventory->AddItem(WeaponDetails, 0);
	if (WeaponDetails.Name == CurrentWeapon.Name) CurrentWeapon.Ammo += WeaponDetails.Ammo;
}

// Called to attack the player
void ABTEnemy::Attack()
{
	// If we have ammo, attack
	if (CurrentWeapon.Ammo > 0)
	{
		// We make sure we are facing the player
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Player->GetActorLocation()));
		
		// This check ensures that the enemy only spawns a laser if the player is far enough away that the laser will not bug out due to start overlaps
		if (UKismetMathLibrary::Vector_Distance(GetActorLocation(), Player->GetActorLocation()) > 300.f)
		{
			// These are the parameters for the laser
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			FVector SpawnLoc = GetActorLocation() + GetActorForwardVector() * 100;
			FRotator SpawnRot = FRotator(0, 0, 0);

			// Spawn a laser and assign its attributes
			ALaser* Laser = GetWorld()->SpawnActor<ALaser>(ALaser::StaticClass(), SpawnLoc, SpawnRot, SpawnParams);
			if (Laser) Laser->SetupLaser(CurrentWeapon.Range, CurrentWeapon.Damage, GetActorForwardVector(), 100.f);
		}
		else // If the player is too close to spawn a laser, then just hurt the player, but play the sound anyway to make it seem like they have been shot
		{
			Player->RecieveAttack(CurrentWeapon.Damage);
		}
		AttackSoundComponent->Play();
		AttackTimer = CurrentWeapon.RateOfFire * 2;
		CurrentWeapon.Ammo--;
	}
	else // If we don't have ammo, flick through the inventory to see if there is any gun with ammo
	{
		// First we save the current weapon
		Inventory->Inventory[CurrentInventoryItem].Ammo = 0;

		// We loop through the inventory and find anything with ammo
		bool AmmoFound = false;
		for (int32 InventoryIndex = 0; InventoryIndex < Inventory->Inventory.Num(); ++InventoryIndex)
		{
			if (Inventory->Inventory[InventoryIndex].Ammo > 0)
			{
				AmmoFound = true;
				CurrentInventoryItem = InventoryIndex;
				break;
			}
		}

		// If we found ammo, equip that weapon
		if (AmmoFound)
		{
			Equip(Inventory->Inventory[CurrentInventoryItem]);
			Attack();
		}
		else
		{
			// TODO: Setup what happens if no ammo
		}
	}
}

// Called to get the closest weapon spawner for the enemy to go to
AActor* ABTEnemy::GetClosestSpawner()
{
	AActor* ClosestSpawner = nullptr;
	for (auto& Spawner : WeaponSpawners)
	{
		if (ClosestSpawner)
		{
			if (UKismetMathLibrary::Vector_Distance(Spawner->GetActorLocation(), GetActorLocation()) < UKismetMathLibrary::Vector_Distance(ClosestSpawner->GetActorLocation(), GetActorLocation()))
				ClosestSpawner = Spawner;
		}
		else ClosestSpawner = Spawner;
	}

	return ClosestSpawner;
}

// Called to inflict damage on the enemy
void ABTEnemy::RecieveAttack(float Damage)
{
	if (Health - Damage <= 0.f)
	{
		OnDeath.Broadcast();
		this->Destroy();
	}
	else Health -= Damage;
}