// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeAttackMission.h"

#include "Checkpoint.h"
#include "DefaultGameState.h"
#include "MissionManager.h"

ATimeAttackMission::ATimeAttackMission()
{
	
}



void ATimeAttackMission::StartEvent()
{
	Super::StartEvent();

	for (auto Checkpoint : Checkpoints)
	{
		Checkpoint->SetMissionReference(this);
	}
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
		if (DefaultGameState) DefaultGameState->GetMissionManager()->EndEvent(this);
	}
	else
	{
		CurrentCheckpointIndex++;
		Checkpoints[CurrentCheckpointIndex]->EnableCheckpoint();
	}
}

