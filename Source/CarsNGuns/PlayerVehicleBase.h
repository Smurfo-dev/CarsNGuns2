// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePhysicsVehiclePawn.h"
#include "PlayerVehicleBase.generated.h"

UENUM(BlueprintType)
enum class ECameraMode : uint8
{
	Isometric UMETA(DisplayName = "Isometric"),
	ThirdPerson UMETA(DisplayName = "Third Person")
};
/**
 * 
 */
UCLASS()
class CARSNGUNS_API APlayerVehicleBase : public ABasePhysicsVehiclePawn
{
	GENERATED_BODY()

public:

	APlayerVehicleBase();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Camera sensitivity for manual targeting
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Input)
	float CameraSensitivity = 1;

	//How fast the camera snaps to target direction
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	float CameraLockSpeed = 4.0f;

	//Minimum range zoom
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	float MinCameraZoom = 750.0f;

	//Max Range zoom
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	float MaxCameraZoom = 2500.0f;

	//how fast the camera zooms
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	float CameraZoomSensitivity = 4.0f;

	//Default camera pitch
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	float DefaultCameraPitch = -35.0f;

	//Lowest Possible Camera Pitch (when zoom is closest to the ground)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinPitch = -15.0f;

	//Range at which camera pitch will be default/max (this value and higher)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	float ZoomRotationalThreshold = 2000.0f;

	//How much the camera yaw will be offset when zoomed in as close as possible
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	float MaxYawOffset = 0.0f;

	//Range at which camera yaw will be default/max (this value and higher)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	float YawTransitionThreshold = 1000.0f;

	//How fast the smoothing effect of zoom is
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "true"))
	float CameraZoomSmoothingSpeed = 5.0f;

	//Is any target visible
	UPROPERTY(BlueprintReadOnly, Category="Targeting")
	bool bTargetVisible = false;

	//The current target of the automatic targeting system
	UPROPERTY(BlueprintReadOnly, Category="Targeting")
	AActor* CurrentTarget;

	UPROPERTY(BlueprintReadOnly, Category="Targeting")
	float EnemyTargetRangeThreshold = 10000.0f;

	UPROPERTY(BlueprintReadWrite, Category="Camera")
	ECameraMode CurrentCameraMode;

	UPROPERTY(BlueprintReadWrite, Category="Camera")
	float DefaultYawOffset;

protected:
	// Called on begin of play
	virtual void BeginPlay() override;
	
	UPROPERTY()
	float TargetZoomLevel;
	
	//CameraComponent
	UPROPERTY(Category="Camera", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(Category="Camera", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	UPROPERTY(Category="Camera", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* CameraRoot;

	UPROPERTY(Category="Camera", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SpringArmLength = 2000.0f;
	
	UPROPERTY(Category="Camera", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector CameraOffset;

	UPROPERTY(Category="Camera", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxCameraDistance;

	//Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ThrottleAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* CameraToggleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* CameraZoomAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* BrakingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SteeringAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* FirePrimaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* FireSecondaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* HandbrakeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* CarFlipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* DebugMenuAction;

private:

	//Input functions
	UFUNCTION()
	void ApplyThrottleInput(const FInputActionValue& Value);
	UFUNCTION()
	void ApplyBrakingInput(const FInputActionValue& Value);
	UFUNCTION()
	void ApplySteeringInput(const FInputActionValue& Value);
	UFUNCTION()
	void ApplyHandbrakeInput(const FInputActionValue& Value);
	UFUNCTION()
	void HandleZoom(const FInputActionValue& Value);
	
	UFUNCTION()
	void FirePrimary(const FInputActionValue& Value);
	UFUNCTION()
	void FireSecondary(const FInputActionValue& Value);

	UFUNCTION()
	void FlipCar();
	
	UFUNCTION()
	void CameraModeToggle();

	UPROPERTY()
	class AMyPlayerController* PlayerController;
	
	FTimerHandle UpdateTargetTimerHandle;

	UPROPERTY()
	UMaterialParameterCollection* ParameterCollection;

	UPROPERTY()
	UMaterialParameterCollectionInstance* ParameterCollectionInstance;

	//Returns true if TargetLocation is within camera bounds
	UFUNCTION()
	bool IsInView(FVector TargetLocation);

	UFUNCTION()
	void UpdateTarget();

	UFUNCTION()
	void OnDeath();

	UFUNCTION()
	void DestroyActor();
	
	UFUNCTION()
	void UpdateCurrentTargetScreenPosition();

	UFUNCTION()
	void ToggleDebugMenu();

	UFUNCTION()
	void UpdatePlayerScreenPosition();

	UFUNCTION()
	void UpdateCameraPitch();

	UFUNCTION()
	void UpdateCameraYaw(float DeltaTime);

	UFUNCTION()
	void UpdateCameraMode();
	
};
