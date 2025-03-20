// Fill out your copyright notice in the Description page of Project Settings.


#include "CarsNGuns/Components/MissionUpgradeComponent.h"

// Sets default values for this component's properties
UMissionUpgradeComponent::UMissionUpgradeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UMissionUpgradeComponent::GetUpgrades()
{
	//Här skapar vi 3 upgrades som är lagliga att appliceras utifrån EUpgradeType Och vad spelaren redan har för upgrades/equippade vapen
	
}


// Called when the game starts
void UMissionUpgradeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMissionUpgradeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

