// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyAIController.h"
#include "TorpedoAIController.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API ATorpedoAIController : public ABaseEnemyAIController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void Follow() override;

	virtual void Torpedo() override;
	
};
