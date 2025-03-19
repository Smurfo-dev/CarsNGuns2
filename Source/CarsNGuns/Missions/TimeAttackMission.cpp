// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeAttackMission.h"

#include "Checkpoint.h"
#include "CarsNGuns/Systems/DefaultGameState.h"
#include "MissionManager.h"

ATimeAttackMission::ATimeAttackMission()
{
	MissionUpgradeComponent->UpgradeType = EUpgradeType::VehicleModification;
	MissionUpgradeComponent->UpgradePreview = FUpgradePreview(MissionUpgradeComponent->UpgradeType, "You will get vehicle Enhancement");
}

void ATimeAttackMission::BeginPlay()
{
	Super::BeginPlay();
}

void ATimeAttackMission::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bMissionStarted) UE_LOG(LogTemp, Warning, TEXT("Mission Time Limit: %.2f"), TimeLimit - GetMissionTime()); 
}

void ATimeAttackMission::StartEvent()
{
	Super::StartEvent();

	for (auto Checkpoint : Checkpoints)
	{
		Checkpoint->SetMissionReference(this);
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Starting Mission: %s & Enabling Checkpoint: %d/%d"), *MissionID, CurrentCheckpointIndex + 1, Checkpoints.Num()));	
	Checkpoints[CurrentCheckpointIndex]->EnableCheckpoint();
}

void ATimeAttackMission::EndEvent(bool bSuccess)
{
	Super::EndEvent(bSuccess);

	for (auto Checkpoint : Checkpoints)
	{
		Checkpoint->SetMissionReference(nullptr);
	}
}

void ATimeAttackMission::ActivateCheckpoint()
{
	Checkpoints[CurrentCheckpointIndex]->DisableCheckpoint();
	if (CurrentCheckpointIndex == Checkpoints.Num() - 1)
	{
		if (DefaultGameState)
		{
			if (GetMissionTime() > TimeLimit) DefaultGameState->GetMissionManager()->EndEvent(this, false);
			else DefaultGameState->GetMissionManager()->EndEvent(this, true);
		}
	}
	else
	{
		CurrentCheckpointIndex++;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Enabling Checkpoint: %d/%d"), CurrentCheckpointIndex + 1, Checkpoints.Num()));	
		Checkpoints[CurrentCheckpointIndex]->EnableCheckpoint();
	}
}

