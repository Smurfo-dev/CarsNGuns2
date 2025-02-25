// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemyAIController.generated.h"

/**
 * 
 */

enum class EAIState
{
	Follow,
	Chase,
	Torpedo,
	Shooting
};

UCLASS()
class CARSNGUNS_API ABaseEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseEnemyAIController();

	//begin play
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

protected:
	
	virtual void Follow();

	void Chase();

	virtual void Torpedo();

	virtual void Shooting();

private:
	EAIState CurrentState;
	
	UPROPERTY()
	AActor* PlayerReference = nullptr;

	UPROPERTY()
	class AEnemyVehicleBase* EnemyVehicleReference = nullptr;

	UPROPERTY(EditAnywhere, Category=AI)
	float TrackingRadius = 1000.0f; //Distance AI will try to follow player at

	void SetState(EAIState NewState);
	
};
