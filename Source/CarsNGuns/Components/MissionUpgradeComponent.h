// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MissionUpgradeComponent.generated.h"

enum class EWeaponType : uint8;

UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
	WeaponAugment      UMETA(DisplayName="Weapon Augment"), // New Weapon Mechanics
	WeaponEnhancement    UMETA(DisplayName="Weapon Enhancement"), // Weapon stat boosts
	VehicleModification   UMETA(DisplayName="Vehicle Modification") // Vehicle Modifications (stats and perhaps mechanics, visual modifications possibly)
};

UENUM(BlueprintType)
enum class EMissionType : uint8
{
	TimeAttack UMETA(DisplayName = "Time Attack")
};

UENUM(BlueprintType)
enum class EStatEnhancementType : uint8
{
	Damage UMETA(DisplayName = "Damage"),
	FireRate UMETA(DisplayName = "Fire Rate"),
	ReloadSpeed UMETA(DisplayName = "Reload Speed"),
	// mer typer perhabs
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

USTRUCT(BlueprintType)
struct FUpgrade
{
	GENERATED_BODY()
	
	EUpgradeType UpgradeType;

	// Only applicable for WeaponEnhancement & WeaponAugment
	TArray<EWeaponType> CompatibleWeaponTypes;

	// Only applicable for WeaponAugment, references the new weapon class
	TSubclassOf<class ABaseWeapon> AugmentedWeaponClass;
	
	EStatEnhancementType StatEnhancementType;
	
	float StatEnhancementValue = 0.0f;
	
	FString UpgradeIconFilePath;
	
	FString DisplayName;
	
	FString UpgradeDescription;

	FUpgrade()
		: UpgradeType(EUpgradeType::WeaponEnhancement), StatEnhancementType(EStatEnhancementType::Damage) {}

	// Parameterized constructor
	FUpgrade(const EUpgradeType Type, const TArray<EWeaponType>& CompatibleWeaponTypes, const TSubclassOf<UObject>& ModClass, const EStatEnhancementType StatType, const float EnhancementValue,
		const FString& IconFilePath, const FString& DisplayName, const FString& UpgradeDescription)
		: UpgradeType(Type), CompatibleWeaponTypes(CompatibleWeaponTypes), AugmentedWeaponClass(ModClass), StatEnhancementType(StatType),  StatEnhancementValue(EnhancementValue),
			UpgradeIconFilePath(IconFilePath), DisplayName(DisplayName), UpgradeDescription(UpgradeDescription) {}
	
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
