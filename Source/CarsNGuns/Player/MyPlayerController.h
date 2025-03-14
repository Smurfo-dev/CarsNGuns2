// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();

	// You can call this function when the player dies
	UFUNCTION()
	void HandlePlayerDeath();

	UFUNCTION()
	void UpdateWeaponIcons();

	UFUNCTION()
	void ToggleDebugMenu();

	void SetupWidgets();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCrosshairWidget> CrosshairWidgetClass;

	UPROPERTY()
	UCrosshairWidget* CurrentCrosshairWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UDebugMenuWidget> DebugMenuWidgetClass;

	UPROPERTY()
	UDebugMenuWidget* CurrentDebugMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UHUDWidget> HUDWidgetClass;

	UPROPERTY()
	UHUDWidget* CurrentHUDWidget;
	
};
