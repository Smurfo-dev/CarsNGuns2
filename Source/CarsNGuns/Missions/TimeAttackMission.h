// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMission.h"
#include "TimeAttackMission.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API ATimeAttackMission : public ABaseMission
{
	GENERATED_BODY()

public:
	ATimeAttackMission();

	virtual void StartEvent() override;

	virtual void EndEvent(bool bSuccess) override;

	virtual void ActivateCheckpoint() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Category="Mission Details")
	float TimeLimit = 60.0f;

	UPROPERTY(EditAnywhere, Category="Mission Details")
	TArray<class ACheckpoint*> Checkpoints;

	int CurrentCheckpointIndex = 0;
	
};
