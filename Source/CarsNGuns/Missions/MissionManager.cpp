// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionManager.h"
#include "BaseMission.h"

// Sets default values
AMissionManager::AMissionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

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

void AMissionManager::StartEvent(ABaseMission* Mission)
{
	for (auto M : Missions)
	{
		if (M->GetMissionState() == EMissionState::Active) M->SetMissionState(EMissionState::Inactive);
	}
	Mission->StartEvent();
}

void AMissionManager::EndEvent(ABaseMission* Mission, bool bSuccess)
{
	Mission->EndEvent(bSuccess);
	for (auto M : Missions)
	{
		if (M->GetMissionState() == EMissionState::Inactive) M->SetMissionState(EMissionState::Active);
	}
}

void AMissionManager::AddMission(ABaseMission* Mission)
{
	if (Mission && !Missions.Contains(Mission))
	{
		Missions.Add(Mission);
		Mission->SetMissionState(EMissionState::Active);
	}
}

void AMissionManager::SetPlayerVehicleReference(ABasePhysicsVehiclePawn* PlayerVehicle)
{
	if (PlayerVehicle)
	{
		PlayerVehicleReference = PlayerVehicle;
	}
}


