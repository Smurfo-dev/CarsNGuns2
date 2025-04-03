// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarsNGuns/Components/UpgradeHandlerComponent.h"
#include "MissionUpgradeComponent.generated.h"

enum class EWeaponType : uint8;

UENUM(BlueprintType)
enum class EMissionType : uint8
{
	TimeAttack UMETA(DisplayName = "Time Attack")
};

USTRUCT(BlueprintType)
struct FMissionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission Preview")
	FString MissionName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission Preview")
	EMissionType MissionType;
	//Should be set by the mission type / mission class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission Preview") 
	EUpgradeType UpgradeType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission Preview")
	FString UpgradeDescription; // Example: "This mission rewards a Weapon Augment!"

	FMissionInfo() : MissionType(EMissionType::TimeAttack), UpgradeType(EUpgradeType::WeaponAugment), UpgradeDescription(TEXT("Default Description")) {}
	
	FMissionInfo(const EMissionType MissionType, const EUpgradeType Type, const FString& Desc) : MissionType(MissionType), UpgradeType(Type), UpgradeDescription(Desc) {}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARSNGUNS_API UMissionUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	class UDefaultGameInstance* DefaultGameInstance;

public:	
	// Sets default values for this component's properties
	UMissionUpgradeComponent();

	//Funktion som ska fylla upgrade choice widgeten med "lagliga" val från AvailableUpgrades mapen.
	void GetUpgrades();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission Info")
	FMissionInfo MissionInfo;

	TArray<FUpgrade> Upgrades;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
