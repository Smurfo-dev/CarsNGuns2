// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DefaultGameState.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API ADefaultGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADefaultGameState();
	
	UFUNCTION(BlueprintCallable)
	class UEnemyManager* GetEnemyManager() const { return EnemyManager; }
	
	UFUNCTION(BlueprintCallable)
	class AMissionManager* GetMissionManager() const { return MissionManager; }

	void PopulateEnemies() const;

	void InitializeMissionManager();

	void SetPlayerController(class AMyPlayerController* Controller)
	{
		PlayerController = Controller;	
	}

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	UEnemyManager* EnemyManager;

	UPROPERTY()
	AMissionManager* MissionManager;

	UPROPERTY()
	AMyPlayerController* PlayerController;
	
};
