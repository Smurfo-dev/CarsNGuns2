// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "BasePhysicsVehiclePawn.generated.h"

UENUM(BlueprintType)
enum class EWheelType : uint8
{
	Comfort  UMETA(DisplayName = "Comfort"),
	OffRoad  UMETA(DisplayName = "Off-Road"),
	Hybrid UMETA(DisplayName = "Hybrid"),
	Track  UMETA(DisplayName = "Track")
};

USTRUCT(BlueprintType)
struct FVehicleStatsUI
{
	GENERATED_BODY()

	// Acceleration Stat (1-10)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Vehicle Stats")
	float Acceleration;

	// Top Speed Stat (1-10)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Vehicle Stats")
	float TopSpeed;

	// Handling Stat (1-10)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Vehicle Stats")
	float Handling;

	// Toughness Stat (1-10)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Vehicle Stats")
	float Toughness;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Vehicle Stats")
	FName VehicleName;

	// Kanske kan tas direkt ifrån blueprint info
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Vehicle Stats")
	FName WheelType;
	
	FVehicleStatsUI() : Acceleration(5.0f), TopSpeed(5.0f), Handling(5.0f), Toughness(5.0f), VehicleName(TEXT("DefaultName")), WheelType(TEXT("DefaultWheels")){}
};

USTRUCT(BlueprintType)
struct FWheelConfig
{
	GENERATED_BODY()

	//Wheel Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WheelConfig")
	EWheelType WheelType;

	//Wheel Display Name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WheelConfig")
	FName DisplayName;

	//Grip Multiplier on asphalt surfaces
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WheelConfig")
	float TarmacGripMultiplier;

	//Grip multiplier on light off road surfaces (eg. gravel)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WheelConfig")
	float LightOffRoadGripMultiplier;

	//Grip multiplier on heavy off road surfaces (eg. mud)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WheelConfig")
	float HeavyOffRoadGripMultiplier;

	//Speed multiplier on tarmac surfaces , multiplies longitudinal friction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WheelConfig")
	float TarmacSlowDownFactor;

	//Speed multiplier on light off road surfaces (eg. gravel), multiplies longitudinal friction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WheelConfig")
	float LightOffRoadSlowDownFactor;

	//Slow down multiplier on heavy off road surfaces (eg. mud), multiplies longitudinal friction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WheelConfig")
	float HeavyOffRoadSlowDownFactor;
};

USTRUCT(BlueprintType)
struct FGearboxConfig
{
	GENERATED_BODY()

	//Max speed for current gear
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GearboxConfig")
	float MaxSpeed; 

	//How quickly the vehicle accelerates for current gear
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GearboxConfig")
	float AccelerationFactor;
};

UCLASS()
class CARSNGUNS_API ABasePhysicsVehiclePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABasePhysicsVehiclePawn();

	//Max suspension extent in centimeters
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Suspension", meta=(AllowPrivateAccess = "true"))
	float MaxSuspensionExtent = 40.0f;

	//Suspension Force Factor multiplied by 1 000 0000
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Suspension", meta=(AllowPrivateAccess = "true"))
	float SuspensionForceFactor = 1.0f;

	//Downforce Factor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Suspension", meta=(AllowPrivateAccess = "true"))
	float DownforceFactor = 1.0f;

	//Suspension Damping Factor multiplied by 1 000
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Suspension", meta=(AllowPrivateAccess = "true"))
	float SuspensionDampingFactor = 2.0f;

	//How much throttle force is being applied
	UPROPERTY(EditAnywhere, Category="Engine")
	float ThrottleForceFactor = 1.0f;

	//How much braking force is being applied
	UPROPERTY(EditAnywhere, Category="Engine")
	float BrakeForceFactor = 1.0f;

	//The RPM where the vehicle will idle
	UPROPERTY(EditAnywhere, Category="Engine")
	float IdleRPM = 1500.0f;

	//Maximum RPM the vehicle can reach
	UPROPERTY(EditAnywhere, Category="Engine")
	float MaxRPM = 7000.0f;

	//The current gear of the vehicle
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Gearbox")
	int CurrentGear = 1;

	//At which RPM the gearbox will shift down
	UPROPERTY(EditAnywhere, Category="Gearbox")
	float DownshiftFactor = 0.3f;

	//Time it takes to shift gears
	UPROPERTY(EditAnywhere, Category="Gearbox")
	float ShiftDelay = 0.15f;

	//Gearbox Speed where gear will shift up & acceleration factor of each gear
	UPROPERTY(EditAnywhere, Category="Gearbox")
	TArray<FGearboxConfig> GearboxConfig = {
		{30.0f, 10.0f},
		{60.0f, 8.0f},
		{100.0f, 6.0f},
		{150.0f, 4.5f},
		{200.0f, 3.0f},
	};

	//How much engine brake is being applied when off throttle
	UPROPERTY(EditAnywhere, Category="Engine")
	float EngineBrakeForceFactor = 1.0f;

	//How much reverse throttle force is being applied when reversing
	UPROPERTY(EditAnywhere, Category="Engine")
	float ReverseForceFactor = 1.0f;

	//0-1.0 Power split between wheels, 1.0 = Only front wheels, 0.5 = Equal distribution
	UPROPERTY(EditAnywhere, Category="Engine")
	float DifferentialRatio = 0.5f;

	//Factor to multiply steering force by
	UPROPERTY(EditAnywhere, Category="Wheels")
	float SteeringTorqueForceFactor = 1.0f;

	//Force of lateral friction multiplier
	UPROPERTY(EditAnywhere, Category="Wheels")
	float LateralFrictionFactor = 1.0f;

	//How much drift assistance force is applied
	UPROPERTY(EditAnywhere, Category="Wheels")
	float DriftAssistFactor = 1.0f;

	//Max Corrective Foward Force During Drift
	UPROPERTY(EditAnywhere, Category="Wheels")
	float MaxCorrectiveForwardForceFactor = 3.0f;
	
	//Max Drift Angle for forces
	UPROPERTY(EditAnywhere, Category="Wheels")
	float MaxDriftAngle = 80.0f;

	//How much drift assistance is boosted by throttle
	UPROPERTY(EditAnywhere, Category="Wheels")
	float ThrottleDriftAssistanceBoost = 10.0f;

	//How much drift correction is boosted by throttle
	UPROPERTY(EditAnywhere, Category="Wheels")
	float ThrottleDriftCorrectionBoost = 10.0f;

	//Force of Longitudinal Friction
	UPROPERTY(EditAnywhere, Category="Wheels")
	float LongitudinalFrictionFactor = 1.0f;

	//Wheel Radius
	UPROPERTY(EditAnywhere, Category="Wheels")
	float WheelRadius = 20.0f;

	//How much force to dampen steering torque by
	UPROPERTY(EditAnywhere, Category="Wheels")
	float SteeringTorqueDampingFactor = 1.0f;

	//Speed at which LongitudinalFrictionForce is strongest
	UPROPERTY(EditAnywhere, Category="Wheels")
	float MaxLongitudinalFrictionSpeed = 200.0f;

	//At which speed the rotation of the vehicle will be significantly dampened
	UPROPERTY(EditAnywhere, Category="Wheels")
	float RotationSpeedThreshold = 5.0f;

	//Float curve of steering angles at certain speeds
	UPROPERTY(EditAnywhere, Category="Wheels")
	UCurveFloat* SteeringSensitivityCurve;

	//Factor to multiply steering sensitivity by
	UPROPERTY(EditAnywhere, Category="Wheels")
	float SteeringSensitivityFactor = 1.0f;

	//Factor to multiply steering sensitivity by while drifting, (increased sensitivity while drifting so > 1.0f)
	UPROPERTY(EditAnywhere, Category="Wheels")
	float DriftSteeringSensitivityFactor = 1.0f;

	//The Current Wheel Config of the vehicle
	UPROPERTY(EditAnywhere, Category="Wheels")
	FWheelConfig WheelConfig;

	//Handbrake force factor
	UPROPERTY(EditAnywhere, Category="Wheels")
	float HandbrakeForceFactor = 1.0f;

	//How powerful the "snapback" force is
	UPROPERTY(EditAnywhere, Category="Wheels")
	float SnapBackTorqueFactor = 1;

	//Debug Forces on the vehicle
	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugForces = true;

	//Debug engine and gearbox values
	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugEngine = true;

	//Debug drifting information
	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugDrifting = true;

	//Should sounds play
	UPROPERTY(EditAnywhere, Category="Debug")
	bool bPlaySound = true;

	UPROPERTY(EditAnywhere, Category="Weapons")
	bool bAutoEquipWeapons = false;

	//Scale of death explosion visual effect
	UPROPERTY(EditAnywhere, Category="VFX")
	int DeathExplosionScaleFactor = 10;

	//Explosion Particle System
	UPROPERTY(Category="VFX", EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* ExplosionParticleSystem;

	//Explosion sound effect
	UPROPERTY(Category="SFX", EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ExplosionSoundCue;

	//Vehicle Statistics for display in UI (1-10)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="UI_Stats")
	FVehicleStatsUI VehicleStats;

	//Gets UI stats of vehicle
	FVehicleStatsUI GetVehicleStats() const { return VehicleStats;}

	UFUNCTION()
	TArray<FVector> GetWheelPositions();

	UFUNCTION()
	void SetSteeringInput(float NewInput);

	UFUNCTION()
	void SetThrottleInput(float NewInput);

	UFUNCTION()
	void SetBrakingInput(float NewInput);

	UFUNCTION()
	void SetHandbrakeInput(bool NewInput);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	UBoxComponent* CollisionBoxPrimitive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	UPoseableMeshComponent* VehicleMeshComponent;

	//EngineAudioComponent
	UPROPERTY(Category="Audio", EditDefaultsOnly, BlueprintReadOnly)
	UAudioComponent* EngineAudioComponent;

	//Set class of primary weapon
	UPROPERTY(EditAnywhere, Category="Primary Weapon")
	TSubclassOf<class ABaseWeapon> PrimaryWeaponClass;

	UPROPERTY(EditAnywhere, Category="Secondary Weapon")
	TSubclassOf<ABaseWeapon> SecondaryWeaponClass;

	UPROPERTY()
	ABaseWeapon* PrimaryWeapon;

	UPROPERTY(EditAnywhere, Category="Primary Weapon", meta=(ToolTip="Reference Joint name for current equipped weapon"))
	FName PrimaryWeaponID;

	UPROPERTY()
	ABaseWeapon* SecondaryWeapon;

	UPROPERTY(EditAnywhere, Category="Secondary Weapon", meta=(ToolTip="Reference Joint name for current equipped weapon"))
	FName SecondaryWeaponID;

	//Health Component
	UPROPERTY(EditDefaultsOnly, Category="Health")
	class UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category="Armor")
	EArmorLevel ArmorLevel;

	UPROPERTY()
	TArray<FVector> WheelPositions;

	UPROPERTY()
	TArray<bool> WheelGroundPositions = {false, false, false, false};

	//Current Throttle Input
	UPROPERTY()
	float ThrottleInput = 0.0f;

	UPROPERTY()
	bool bLockUpVehicle = false;

	UPROPERTY()
	bool bIsShifting = false;

	UPROPERTY()
	bool bIsRedlining = false;

	UPROPERTY()
	TArray<UPhysicalMaterial*> CurrentSurfaceMaterials = {nullptr, nullptr, nullptr, nullptr};

	//Current Braking Input
	UPROPERTY()
	float BrakingInput = 0.0f;

	//Current Speed of vehicle
	UPROPERTY()
	float CurrentSpeed;

	//Current Forward Direcitonal Speed of vehicle
	UPROPERTY()
	float CurrentForwardSpeed;

	UPROPERTY()
	FVector RoadNormal = FVector::ZeroVector;

	UPROPERTY()
	float CurrentSteeringInput;

	//Current Handbrake Input
	UPROPERTY()
	bool bHandbrake = false;

	UPROPERTY()
	bool bIsDead = false;

	UPROPERTY()
	FTimerHandle DestroyTimerHandle;

	UPROPERTY()
	bool bIsDrifting = false;

	UPROPERTY()
	bool bIsReversing = false;

	// Minimum speed to allow steering
	UPROPERTY()
	float SpeedThreshold = 3.0f;
	
	//Position on screen of current target
	UPROPERTY(BlueprintReadOnly, Category="Targeting")
	FVector2D CurrentTargetScreenPosition;

	//Current Camera Lock Mode
	UPROPERTY(BlueprintReadOnly, Category="Targeting")
	bool bCameraLock = false;

	bool bIsInAir = false;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector2D GetCurrentTargetScreenPosition() const;

	bool GetCurrentCameraLockSetting() const;

	void AttachWeaponToVehicle(const TSubclassOf<ABaseWeapon>& WeaponClass);

private:

	UPROPERTY()
	TArray<float> SuspensionHeights = {0, 0, 0, 0};

	UPROPERTY()
	float CurrentRPM = IdleRPM;
	
	UPROPERTY()
	float SteeringSpeedAmplificationFactor = 0.1f;

	UPROPERTY()
	FTimerHandle ShiftTimerHandle;

	void CalculateSuspensionForces();

	void CalculateTractionForces();

	void ApplySteeringTorque();

	void ApplyThrottle();

	void ApplyBraking();

	void ApplyHandbrake();

	void ApplyFrictionForces();

	void ApplyDownforce();

	void UpdateEngine();

	void UpdateRPM();

	void UpdateGear();

	void UpdateWheelAnimations(float DeltaTime);

	void ResetShifting(){bIsShifting = false;}

	int GetCurrentGear();

	void UpdateEngineSFX();

	float CalculateDriftAngle() const;

	void InitAudio();

	//Returns True if all wheels are touching ground
	UFUNCTION()
	bool IsGrounded();

	//Returns True if no wheels are touching ground
	UFUNCTION()
	void IsInAir();

};
