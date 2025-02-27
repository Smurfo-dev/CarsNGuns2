// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API AGameplayGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameplayGameMode();
	
	//Returns current Level Timer
	UFUNCTION(BlueprintCallable, Category = "Timer")
	float GetLevelTime() const;

	void SetTimerActive(bool bActive);

protected:
	virtual void BeginPlay() override;

	//InputMappingContext
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MappingContext")
	class UInputMappingContext* VehicleMappingContext;

	UPROPERTY()
	TSubclassOf<class ABasePhysicsVehiclePawn> SelectedPlayerPawnClass;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> WeaponSelectionMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<class ABaseWeapon>> AvailableWeapons;

	float StartTime;  // Stores the world time when the timer starts
	float PausedTime; // Tracks the time when paused
	bool bIsTimerActive;

	UFUNCTION()
	void ShowWeaponSelectionMenu();

	UFUNCTION()
	void SetupPlayer();

	UFUNCTION()
	void OnWeaponSelectionMenuClosed();

	UFUNCTION()
	void SetupInputAfterDelay();
	
};
