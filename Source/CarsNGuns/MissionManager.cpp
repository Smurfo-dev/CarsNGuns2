// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionManager.h"

// Sets default values
AMissionManager::AMissionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	CurrentCheckpointIndex = 0;

}

void AMissionManager::StartEvent(FName EventID)
{
	if(!bMissionActive)
	{
		CurrentMission = MissionDatabase.Find(EventID);
		ActiveCheckpoint = CurrentMission->Checkpoints[0];
		bMissionActive = true;
		UE_LOG(LogTemp, Log, TEXT("Mission Started"))
	}
	
}

void AMissionManager::OnCheckPointReached()
{
	if(CurrentCheckpointIndex == CurrentMission->Checkpoints.Num() - 1)
	{
		EndEvent(CurrentMission->MissionID);
		return;
	}
	CurrentCheckpointIndex++;
	ActiveCheckpoint = CurrentMission->Checkpoints[CurrentCheckpointIndex];
	UE_LOG(LogTemp, Log, TEXT("Current Checkpoint, %d"), CurrentCheckpointIndex)
}

void AMissionManager::EndEvent(FName EventID)
{
	ActiveCheckpoint = nullptr;
	CurrentMission = nullptr;
	bMissionActive = false;
	//Return to open world, get rewards,
	UE_LOG(LogTemp, Log, TEXT("Mission Over"))
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

