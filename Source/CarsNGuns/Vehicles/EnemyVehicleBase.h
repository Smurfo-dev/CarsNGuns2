// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePhysicsVehiclePawn.h"
#include "EnemyVehicleBase.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API AEnemyVehicleBase : public ABasePhysicsVehiclePawn
{
	GENERATED_BODY()
public:
	AEnemyVehicleBase();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Behavior")
	float MaxSpawnDistance = 10000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning Behavior")
	float MinSpawnDistance = 5000.0f;

private:
	UFUNCTION()
	void OnDeath();

	UFUNCTION()
	void DestroyActor();
	
};
