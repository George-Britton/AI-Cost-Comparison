// Fill out your copyright notice in the Description page of Project Settings.


#include "FSMEnemy.h"
#include "../Pickups/SpawnerBase.h"
#include "Sound/SoundCue.h"
#include "Laser.h"
#include "GameFramework/CharacterMovementComponent.h"

// Global constants
static const float ANIMATION_MESH_OFFSET = -85.f; // The offset for the character mesh to be on the ground
static const float GUN_OFFSET = 20.f; // The offset for the gun to be in front
// 
// Sets default values
AFSMEnemy::AFSMEnemy()
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
void AFSMEnemy::BeginPlay()
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
void AFSMEnemy::CreateInventory()
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
void AFSMEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to equip a weapon
void AFSMEnemy::Equip(FWeaponDetails Weapon)
{
	CurrentWeapon = Weapon;
	RangedMeshComponent->SetSkeletalMesh(CurrentWeapon.RangedMesh);
}
void AFSMEnemy::PickupWeapon(FWeaponDetails WeaponDetails, AActor* Actor)
{
	Inventory->AddItem(WeaponDetails, 0);
	if (WeaponDetails.Name == CurrentWeapon.Name) CurrentWeapon.Ammo += WeaponDetails.Ammo;
}