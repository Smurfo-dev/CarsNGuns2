// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"

#include "RoadGenerator.h"
#include "CarsNGuns/Player/MyPlayerController.h"
#include "CarsNGuns/Vehicles/EnemyVehicleBase.h"

void AEnemyManager::AddEnemy(AActor* NewEnemy)
{
	if (NewEnemy && !Enemies.Contains(NewEnemy))
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Enemy"));
		Enemies.Add(NewEnemy);
	}
}

void AEnemyManager::RemoveEnemy(AActor* EnemyToRemove)
{
	UE_LOG(LogTemp, Warning, TEXT("Removing Enemy"));
	Enemies.Remove(EnemyToRemove);
}

void AEnemyManager::ClearEnemies()
{
	if(!Enemies.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Clearing Enemy Array"));
		Enemies.Empty();
	}
	
}

void AEnemyManager::SpawnEnemy(AMyPlayerController* PlayerController, TSubclassOf<AEnemyVehicleBase> EnemyClass)
{
	if (PlayerController && EnemyClass)
	{
		
		if (APawn* PlayerPawn = PlayerController->GetPawn())
		{
			FVector PlayerLocation = PlayerPawn->GetActorLocation();
			FVector PlayerForward = PlayerPawn->GetActorForwardVector();
			AEnemyVehicleBase* DefaultEnemyClass = EnemyClass.GetDefaultObject();

			if (RoadManager)
			{
				
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("No Road Manager Found!"));
			}
			
		}
	}
}
