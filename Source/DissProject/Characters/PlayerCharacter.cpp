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
	InputComponent->BindAction<FToggleState>("Run", EInputEvent::IE_Pressed, this, &APlayerCharacter::ToggleRun, true);
	InputComponent->BindAction<FToggleState>("Run", EInputEvent::IE_Released, this, &APlayerCharacter::ToggleRun, false);
	InputComponent->BindAction<FToggleState>("Crouch", EInputEvent::IE_Pressed, this, &APlayerCharacter::ToggleCrouch, true);
	InputComponent->BindAction<FToggleState>("Crouch", EInputEvent::IE_Released, this, &APlayerCharacter::ToggleCrouch, false);
	InputComponent->BindAction("Action", EInputEvent::IE_Released, this, &APlayerCharacter::Action);
	//InputComponent->BindAction<FToggleState>("Fire", EInputEvent::IE_Pressed, Gun, &UGun::ToggleFire, true);
	//InputComponent->BindAction<FToggleState>("Fire", EInputEvent::IE_Released, Gun, &UGun::ToggleFire, false);
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