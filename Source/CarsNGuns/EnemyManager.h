// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyVehicleBase.h"
#include "UObject/NoExportTypes.h"
#include "EnemyManager.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API UEnemyManager : public UObject
{
	GENERATED_BODY()

public:
	// Add an enemy to the list.
	void AddEnemy(AActor* NewEnemy);

	// Remove an enemy from the list.
	void RemoveEnemy(AActor* EnemyToRemove);

	// Clear list of enemies
	void ClearEnemies();

	// Get the list of enemies.
	const TArray<AActor*>& GetEnemies() const { return Enemies; }

private:
	UPROPERTY()
	TArray<AActor*> Enemies;
	
};
