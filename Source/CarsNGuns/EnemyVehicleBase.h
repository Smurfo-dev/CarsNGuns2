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

private:
	UFUNCTION()
	void OnDeath();

	UFUNCTION()
	void DestroyActor();

	UPROPERTY(EditAnywhere, Category="Debug")
	class USceneCaptureComponent2D* PiPCamera;
	
};
