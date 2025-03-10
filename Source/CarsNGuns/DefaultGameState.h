// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BasePhysicsVehiclePawn.h"
#include "DefaultGameState.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API ADefaultGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	class UEnemyManager* GetEnemyManager() const { return EnemyManager; }
	
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<class ABasePhysicsVehiclePawn> SelectedPlayerPawnClass;

	UFUNCTION(BlueprintCallable)
	void SetSelectedPlayerPawnClass(TSubclassOf<ABasePhysicsVehiclePawn> PawnClass)
	{
		SelectedPlayerPawnClass = PawnClass;
		UE_LOG(LogTemp, Warning, TEXT("Selected pawn class stored: %s"), *PawnClass->GetName());
	}

	UFUNCTION(BlueprintCallable)
	TSubclassOf<ABasePhysicsVehiclePawn> GetSelectedPlayerPawnClass() const
	{
		return SelectedPlayerPawnClass;
	}

	void PopulateEnemies() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UEnemyManager* EnemyManager;
	
};
