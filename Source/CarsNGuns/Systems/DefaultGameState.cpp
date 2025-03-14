// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameState.h"

#include "CarsNGuns/Missions/BaseMission.h"
#include "CarsNGuns/Vehicles/EnemyVehicleBase.h"
#include "EnemyManager.h"
#include "CarsNGuns/Missions/MissionManager.h"
#include "Kismet/GameplayStatics.h"

ADefaultGameState::ADefaultGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADefaultGameState::BeginPlay()
{
	Super::BeginPlay();

	// Create and initialize the EnemyManager.
	EnemyManager = NewObject<UEnemyManager>(this);
}

void ADefaultGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADefaultGameState::PopulateEnemies() const
{
	if(EnemyManager)
	{
		//Clear enemy list initially
		EnemyManager->ClearEnemies();
		
		UE_LOG(LogTemp, Warning, TEXT("EnemyManager Exists"));
		TArray<AActor*> FoundEnemies;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyVehicleBase::StaticClass(), FoundEnemies);
		for(AActor* Enemy : FoundEnemies)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found enemy: %s, Class: %s"), *Enemy->GetName(), *Enemy->GetClass()->GetName());
			EnemyManager->AddEnemy(Enemy);
		}
	}
}

void ADefaultGameState::InitializeMissionManager()
{
	if (AActor* FoundManager = UGameplayStatics::GetActorOfClass(GetWorld(), AMissionManager::StaticClass()))
	{
		MissionManager = Cast<AMissionManager>(FoundManager);
		TArray<AActor*> FoundMissions;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseMission::StaticClass(), FoundMissions);
		
		UE_LOG(LogTemp, Warning, TEXT("MissionManager Found and Bound"));
		for (const auto Mission : FoundMissions)
		{
			if (ABaseMission* MissionToAdd = Cast<ABaseMission>(Mission))
			{
				MissionManager->AddMission(MissionToAdd);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to cast Mission To Base Mission"));
			}
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MissionManager Doesn't Exist in world"));
	}
}

