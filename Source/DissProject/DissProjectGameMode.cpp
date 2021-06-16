// Copyright Epic Games, Inc. All Rights Reserved.

#include "DissProjectGameMode.h"
#include "DissProjectHUD.h"
#include "DissProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADissProjectGameMode::ADissProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ADissProjectHUD::StaticClass();
}
