// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BaseWeapon.h"
#include "BasePhysicsVehiclePawn.h"
#include "DefaultGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<ABaseWeapon>> AvailableWeapons;

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

	void StartTimer();
	void PauseTimer();
	void ResumeTimer();
	void StopTimer();
	float GetElapsedTime() const;


protected:
	virtual void Init() override;

private:
	float StartTime = 0.0f;
	float PausedElapsedTime = 0.0f;
	bool bIsPaused = true;
	
};

