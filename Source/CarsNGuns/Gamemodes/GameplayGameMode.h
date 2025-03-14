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

	UFUNCTION()
	void ShowWeaponSelectionMenu();

	UFUNCTION()
	void SetupPlayer();

	UFUNCTION()
	void OnWeaponSelectionMenuClosed();

	UFUNCTION()
	void SetupInputAfterDelay();
	
};
