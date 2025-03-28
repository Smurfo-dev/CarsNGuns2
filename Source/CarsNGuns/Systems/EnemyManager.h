// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyManager.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API AEnemyManager : public AActor
{
	GENERATED_BODY()

public:
	// Add an enemy to the list.
	void AddEnemy(AActor* NewEnemy);

	// Remove an enemy from the list.
	void RemoveEnemy(AActor* EnemyToRemove);

	void SetRoadManager(class ARoadManager* NewRoadManager)
	{
		RoadManager = NewRoadManager;
	}

	// Clear list of enemies
	void ClearEnemies();

	// Get the list of enemies.
	const TArray<AActor*>& GetEnemies() const { return Enemies; }

	UFUNCTION(BlueprintCallable)
	void SpawnEnemy(class AMyPlayerController* PlayerController, TSubclassOf<class AEnemyVehicleBase> EnemyClass);


private:
	UPROPERTY()
	TArray<AActor*> Enemies;

	UPROPERTY()
	ARoadManager* RoadManager;

	bool IsPointInPlayerView(AMyPlayerController* PlayerController, const FVector& Point);
	
};
