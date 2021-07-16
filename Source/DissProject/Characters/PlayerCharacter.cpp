// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// global constants
const static float MAX_CAMERA_HEIGHT = 100.f;
const static float MAX_CAMERA_PITCH_LIMIT = 90.f;

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DamageSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Damage Sound Component"));
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	PlayerCamera->SetupAttachment(this->RootComponent);
	PlayerCamera->SetRelativeLocation(FVector(0, 0, CameraHeight));
	DamageSoundComponent->SetupAttachment(this->RootComponent);
	DamageSoundComponent->SetAutoActivate(false);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	CreateInventory();

	// We make sure the player possesses the actor, and set the basic input settings
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
}

// Called when a value changes
void APlayerCharacter::OnConstruction(const FTransform& Transform)
{
	// This sets the player damage sound to be played on damage taken
	if (DamageSound) DamageSoundComponent->SetSound(DamageSound);

	// Here we'll make sure all our values stay in a valid range
	CameraHeight = FMath::Clamp(CameraHeight, 1.f, MAX_CAMERA_HEIGHT);
	CameraPitchLimit = FMath::Clamp(CameraPitchLimit, 1.f, MAX_CAMERA_PITCH_LIMIT);
	RunSpeed = FMath::Clamp(RunSpeed, 1.f, RunSpeed);
	CrouchSpeed = FMath::Clamp(CrouchSpeed, 1.f, CrouchSpeed);
	BaseSpeed = FMath::Clamp(BaseSpeed, 1.f, BaseSpeed);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Give the components the correct weapon transforms
	RangedMesh->AddLocalTransform(WeaponViewportTransform);
	MeleeMesh->AddLocalTransform(WeaponViewportTransform);

	NextInventory(0);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Here we bind the player's inputs to the actions' code
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveX);
	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveY);
	InputComponent->BindAxis("RotateX", this, &APlayerCharacter::RotateX);
	InputComponent->BindAxis("RotateY", this, &APlayerCharacter::RotateY);
	InputComponent->BindAction<FToggleInventory>("NextInventory", EInputEvent::IE_Pressed, this, &APlayerCharacter::NextInventory, 1);
	InputComponent->BindAction<FToggleInventory>("PreviousInventory", EInputEvent::IE_Pressed, this, &APlayerCharacter::NextInventory, -1);
	InputComponent->BindAction<FToggleState>("Run", EInputEvent::IE_Pressed, this, &APlayerCharacter::ToggleRun, true);
	InputComponent->BindAction<FToggleState>("Run", EInputEvent::IE_Released, this, &APlayerCharacter::ToggleRun, false);
	InputComponent->BindAction<FToggleState>("Crouch", EInputEvent::IE_Pressed, this, &APlayerCharacter::ToggleCrouch, true);
	InputComponent->BindAction<FToggleState>("Crouch", EInputEvent::IE_Released, this, &APlayerCharacter::ToggleCrouch, false);
	InputComponent->BindAction("Action", EInputEvent::IE_Released, this, &APlayerCharacter::Action);
	//InputComponent->BindAction<FToggleState>("Fire", EInputEvent::IE_Pressed, Gun, &AWeapon::Attack, true);
	//InputComponent->BindAction<FToggleState>("Fire", EInputEvent::IE_Released, Gun, &AWeapon::Attack, false);
}

// Called to set up the inventory
void APlayerCharacter::CreateInventory()
{
	// Create the meshes components and add them to root
	MeleeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Melee Mesh"));
	RangedMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Ranged Mesh"));
	MeleeMesh->SetupAttachment(this->PlayerCamera);
	RangedMesh->SetupAttachment(this->PlayerCamera);

	// Create the inventory object
	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));

	// Create a knife weapon and add it to the inventory
	static ConstructorHelpers::FObjectFinder<UStaticMesh> KnifeAsset(TEXT("/Game/PolygonScifi/Meshes/Weapons/Accessories/SM_Wep_Knife_01.SM_Wep_Knife_01"));
	FWeaponDetails Knife = FWeaponDetails::FWeaponDetails("Knife", KnifeAsset.Object, nullptr, 10, 0.f, 0.f, 0.f, EWeaponType::MELEE);
	Inventory->AddItem(&Knife);
}

// INPUTS
// Makes the player move left or right
void APlayerCharacter::MoveX(float AxisValue) { if (AxisValue) AddMovementInput(GetActorRightVector(), AxisValue); }
// Makes the player move forward or backward
void APlayerCharacter::MoveY(float AxisValue) { if (AxisValue) AddMovementInput(GetActorForwardVector(), AxisValue); }
// Makes the player look left or right
void APlayerCharacter::RotateX(float AxisValue) { if (AxisValue) AddActorLocalRotation(FRotator(0, AxisValue, 0)); }
// Makes the player look up or down
void APlayerCharacter::RotateY(float AxisValue)
{
	if (AxisValue)
	{
		float CurrentRotation = PlayerCamera->GetRelativeRotation().Pitch + AxisValue;
		if (CurrentRotation < CameraPitchLimit && CurrentRotation > -CameraPitchLimit) PlayerCamera->AddLocalRotation(FRotator(AxisValue, 0, 0));
	}
}
// Makes the player run
void APlayerCharacter::ToggleRun(bool Running)
{
	// If the player is not running but should be, the player stops crouching and starts running
	if (Running)
	{
		if (!IsRunning)
		{
			ToggleCrouch(false);
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
			IsRunning = true;
		}
	}
	else // otherwise if the player shouldn't be running but are, they stop running
	{
		if (IsRunning)
		{
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
			IsRunning = false;
		}
	}

}
// Crouches the player down
void APlayerCharacter::ToggleCrouch(bool Crouching)
{
	// If the player should be crouching, they stop running and/or jumping, and start crouching
	if (Crouching)
	{
		if (!IsCrouching)
		{
			GetCapsuleComponent()->SetCapsuleSize(GetCapsuleComponent()->GetUnscaledCapsuleRadius(), GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() / 2);
			GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
			IsCrouching = true;
		}
	}
	else // otherwise, if the player shouldn't be crouching but is, they stand up
	{
		if (IsCrouching)
		{
			GetCapsuleComponent()->SetCapsuleSize(GetCapsuleComponent()->GetUnscaledCapsuleRadius(), GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 2);
			GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
			IsCrouching = false;
		}
	}
}
// Announces that the action button has been pressed
void APlayerCharacter::Action()
{
	// First we announce that the action button's been pressed
	OnActionPressed.Broadcast();

	// And then we check if the player is overlapping, and if so, we activate its effect
	if (OverlappedPickup) OverlappedPickup->Activate();
}
// Changes the equipped inventory item
void APlayerCharacter::NextInventory(int32 Change)
{
	if (Change) PrintDebugMessage("NextInventory");

	// Change the current item
	if (Change > 0) ++CurrentInventoryItem;
	if (Change < 0) --CurrentInventoryItem;

	// Wrap the item around the valid indecies
	if (CurrentInventoryItem < 0) CurrentInventoryItem = Inventory->Inventory.Num() - 1;
	if (CurrentInventoryItem == Inventory->Inventory.Num()) CurrentInventoryItem = 0;

	// Get the inventory item
	CurrentWeapon = Inventory->Inventory[CurrentInventoryItem];

	// Set the mesh of the weapon
	switch (CurrentWeapon.Type)
	{
	case EWeaponType::MELEE:
		MeleeMesh->SetStaticMesh(CurrentWeapon.MeleeMesh);
		RangedMesh->SetVisibility(false);
		MeleeMesh->SetVisibility(true);
		break;
	case EWeaponType::RANGED:
		RangedMesh->SetSkeletalMesh(CurrentWeapon.RangedMesh);
		RangedMesh->SetVisibility(true);
		MeleeMesh->SetVisibility(false);
		break;
	default: break;
	}
}

// EFFECTS
// Used to tell the world that money has been collected
void APlayerCharacter::PickUp(FWeaponDetails* InWeapon, int32 InMoney)
{
	// First we update the player's inventory
	Inventory->AddItem(InWeapon, InMoney);

	// And then we announce the new money
	if (InMoney != 0) OnMoneyUpdate.Broadcast(Inventory->Money);

}
// Used to tell the world a weapon has been drawn
void APlayerCharacter::DrawWeapon()
{
	OnWeaponDraw.Broadcast(true);
}

// ATTACKS
// Used to tell the player they have been attacked by a zombie
void APlayerCharacter::RecieveAttack(float Damage)
{
	Health -= Damage;
	DamageSoundComponent->Play();
	OnDamage.Broadcast(Health);
	if (Health <= 0.f)
	{
		OnDamage.Clear();
		OnPlayerDeath.Broadcast();
	}
}