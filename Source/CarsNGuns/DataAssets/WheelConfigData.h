// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WheelConfigData.generated.h"

UENUM(BlueprintType)
enum class EUpgradeRarity : uint8
{
	Common UMETA(DisplayName="Common"),
	Rare UMETA(DisplayName="Rare"),
	Epic UMETA(DisplayName="Epic"),
	Legendary UMETA(DisplayName="Legendary"),
	Mythic UMETA(DisplayName="Mythic")
};

UENUM(BlueprintType)
enum class EWheelType : uint8
{
	Comfort  UMETA(DisplayName = "Comfort"),
	OffRoad  UMETA(DisplayName = "Off-Road"),
	Hybrid UMETA(DisplayName = "Hybrid"),
	Track  UMETA(DisplayName = "Track")
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

/**
 * 
 */
UCLASS()
class CARSNGUNS_API UWheelConfigData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WheelConfig")
	EWheelType WheelType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WheelConfig")
	EUpgradeRarity Rarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WheelConfig")
	FName DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WheelConfig")
	float TarmacGripMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WheelConfig")
	float LightOffRoadGripMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WheelConfig")
	float HeavyOffRoadGripMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WheelConfig")
	float TarmacSlowDownFactor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WheelConfig")
	float LightOffRoadSlowDownFactor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WheelConfig")
	float HeavyOffRoadSlowDownFactor;
	
};
