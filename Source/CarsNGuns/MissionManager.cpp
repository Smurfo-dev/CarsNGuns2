// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionManager.h"

#include "BaseMission.h"

// Sets default values
AMissionManager::AMissionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

}

void AMissionManager::StartEvent(ABaseMission* Mission)
{
	Mission->StartEvent();
}

void AMissionManager::EndEvent(ABaseMission* Mission)
{
	Mission->EndEvent(true);
}

void AMissionManager::AddMission(ABaseMission* Mission)
{
	if (Mission && !Missions.Contains(Mission))
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Enemy"));
		Missions.Add(Mission);
		Mission->SetMissionState(EMissionState::Active);
	}
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

