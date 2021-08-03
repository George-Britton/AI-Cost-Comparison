// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "NavigationAffectingISM.generated.h"

/**
 * 
 */
UCLASS()
class DISSPROJECT_API UNavigationAffectingISM : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UNavigationAffectingISM(){ this->SetCanEverAffectNavigation(true); };
};
