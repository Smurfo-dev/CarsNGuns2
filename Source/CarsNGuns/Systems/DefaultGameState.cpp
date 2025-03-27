// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameState.h"

#include "CarsNGuns/Missions/BaseMission.h"
#include "CarsNGuns/Vehicles/EnemyVehicleBase.h"
#include "EnemyManager.h"
#include "RoadGenerator.h"
#include "RoadManager.h"
#include "CarsNGuns/Missions/MissionManager.h"
#include "Kismet/GameplayStatics.h"

ADefaultGameState::ADefaultGameState()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADefaultGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ADefaultGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADefaultGameState::InitializeEnemyManager()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	// Create and initialize the EnemyManager.
	EnemyManager = GetWorld()->SpawnActor<AEnemyManager>(AEnemyManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	
	if (RoadManager)
	{
		EnemyManager->SetRoadManager(RoadManager);
		//If the road manager has been created give the enemy manager a reference to it.
	}
	
	if(EnemyManager)
	{
		//Clear enemy list initially
		EnemyManager->ClearEnemies();
		
		UE_LOG(LogTemp, Warning, TEXT("EnemyManager Created"));
		TArray<AActor*> FoundEnemies;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyVehicleBase::StaticClass(), FoundEnemies);
		for(AActor* Enemy : FoundEnemies)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found enemy: %s, Class: %s"), *Enemy->GetName(), *Enemy->GetClass()->GetName());
			EnemyManager->AddEnemy(Enemy);
		}
	}
}

void ADefaultGameState::InitializeRoadManager()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	// Create and initialize the EnemyManager.
	RoadManager = GetWorld()->SpawnActor<ARoadManager>(ARoadManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	
	if (RoadManager)
	{
		RoadManager->ClearRoadArray();
		
		UE_LOG(LogTemp, Warning, TEXT("RoadManager Created"));
		TArray<AActor*> FoundRoads;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoadGenerator::StaticClass(), FoundRoads);
		for(AActor* Road : FoundRoads)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Road: %s, Class: %s"), *Road->GetName(), *Road->GetClass()->GetName());
			RoadManager->AddRoad(Road);
		}
	}
}

void ADefaultGameState::InitializeMissionManager(ABasePhysicsVehiclePawn* PlayerVehicleReference)
{
	if (!MissionManager)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		MissionManager = GetWorld()->SpawnActor<AMissionManager>(AMissionManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		MissionManager->SetPlayerVehicleReference(PlayerVehicleReference);
		TArray<AActor*> FoundMissions;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseMission::StaticClass(), FoundMissions);
		
		UE_LOG(LogTemp, Warning, TEXT("MissionManager Created"));
		UE_LOG(LogTemp, Warning, TEXT("Amount Of Found Missions: %d"), FoundMissions.Num());
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
		UE_LOG(LogTemp, Warning, TEXT("MissionManager Already Exists in world"));
	}
}

