// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePhysicsVehiclePawn.h"
#include "EnemyManager.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	UEnemyManager* GetEnemyManager() const { return EnemyManager; }
	
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<ABasePhysicsVehiclePawn> SelectedPlayerPawnClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<class ABaseWeapon>> AvailableWeapons;

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
	virtual void Init() override;

private:
	UPROPERTY()
	UEnemyManager* EnemyManager;
	
};

