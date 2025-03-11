// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionManager.h"

// Sets default values
AMissionManager::AMissionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

}

void AMissionManager::StartEvent(TSubclassOf<ABaseMission> MissionType, FVector Location)
{
	
}

void AMissionManager::EndEvent(ABaseMission* Mission)
{
	
}

// Called when the game starts or when spawned
void AMissionManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMissionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

