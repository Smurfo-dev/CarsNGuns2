// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"

#include "RoadManager.h"
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

			

			// Pick a random distance behind the player
			float SpawnDistance = FMath::RandRange(DefaultEnemyClass->MinSpawnDistance, DefaultEnemyClass->MaxSpawnDistance);
			FVector SpawnPoint = PlayerLocation - PlayerForward * SpawnDistance;

			FVector ClosestRoadPoint;
			if (RoadManager->FindClosestPointOnSpline(SpawnPoint, ClosestRoadPoint))
			{
				if (!IsPointInPlayerView(PlayerController, ClosestRoadPoint))
				{
					FRotator SpawnRotation = RoadManager->GetRoadDirectionAtPoint(ClosestRoadPoint);

					if (AEnemyVehicleBase* Enemy = GetWorld()->SpawnActor<AEnemyVehicleBase>(EnemyClass, ClosestRoadPoint, SpawnRotation))
					{
						AddEnemy(Enemy);
					}
					UE_LOG(LogTemp, Warning, TEXT("Spawned enemy at: %s"), *ClosestRoadPoint.ToString());
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Spawn point was in view, retrying..."));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Couldn't find a valid road point for spawning."));
			}
			
		}
	}
}

bool AEnemyManager::IsPointInPlayerView(AMyPlayerController* PlayerController, const FVector& Point)
{
	if (PlayerController)
	{
		FVector PlayerLocation;
		FRotator PlayerRotation;
		PlayerController->GetPlayerViewPoint(PlayerLocation, PlayerRotation);

		FVector DirectionToPoint = (Point-PlayerLocation).GetSafeNormal();
		FVector PlayerForward = PlayerRotation.Vector();
		float DotProduct = FVector::DotProduct(PlayerForward, DirectionToPoint);
		
		return DotProduct > 0.3f;
	}
	
	return false;
}
