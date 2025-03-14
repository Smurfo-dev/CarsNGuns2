// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyAIController.h"
#include "GunnerAIController.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API AGunnerAIController : public ABaseEnemyAIController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void Shooting() override;

	virtual void TransitionFromFollow() override;

private:
	bool bIsFiring = false;
	
};
