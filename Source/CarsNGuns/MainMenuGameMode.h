// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BasePhysicsVehiclePawn.h"
#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AMainMenuGameMode();
	
	UPROPERTY(EditAnywhere, Category="Camera")
	float CameraMovementSpeed = 2.0f;

	UPROPERTY(EditAnywhere, Category="AvailableVehicles")
	TArray<TSubclassOf<class ABasePhysicsVehiclePawn>> AvailableVehicleClasses;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY()
	UUserWidget* MainMenuWidget;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> ChooseVehicleWidgetClass;

	UPROPERTY()
	UUserWidget* ChooseVehicleWidget;

	UPROPERTY(EditDefaultsOnly, Category="Camera")
	class ACineCameraActor* CinematicCamera;

	UFUNCTION()
	void OnStartGamePressed();

	UFUNCTION()
	void OnExitGamePressed();

	UFUNCTION()
	void OnPlayPressed();
	
	UFUNCTION()
	void OnMainMenuPressed();
	
	UFUNCTION()
	void OnMoveLeftPressed();
	
	UFUNCTION()
	void OnMoveRightPressed();

	UPROPERTY(EditAnywhere)
	FName LevelToOpenOnStartGame;

private:

	//Initial Location for menu camera, is set in begin play
	UPROPERTY(VisibleAnywhere, Category="Camera")
	FVector BaseCameraLocation;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	FRotator BaseCameraRotation;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	FVector TargetCameraLocation;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	FRotator TargetCameraRotation;

	UPROPERTY(VisibleAnywhere, Category="Camera")
	FVector LatestCameraLocation = FVector(2000, -200, 100);

	UPROPERTY(VisibleAnywhere, Category="Camera")
	FRotator LatestCameraRotation = FRotator(0, 90, 0);

	UPROPERTY()
	class UProgressBar* AccelerationStatBar;
	
	UPROPERTY()
	UProgressBar* TopSpeedStatBar;
	
	UPROPERTY()
	UProgressBar* HandlingStatBar;
	
	UPROPERTY()
	UProgressBar* ToughnessStatBar;
	
	UPROPERTY()
	class UTextBlock* VehicleNameTextBlock;
	
	UPROPERTY()
	UTextBlock* WheelTypeNameTextBlock;

	void UpdateVehicleInfo();

	int VehicleIndex = 0;

	bool bCameraIsMoving = false;
	
	void MoveCameraToVehicles();

	void MoveCameraToTitle();

	void MoveCameraLeft();

	void MoveCameraRight();
	
};
