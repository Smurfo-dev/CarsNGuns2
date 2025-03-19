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
enum class EStatEnhancementType : uint8
{
	Damage UMETA(DisplayName = "Damage"),
	FireRate UMETA(DisplayName = "Fire Rate"),
	ReloadSpeed UMETA(DisplayName = "Reload Speed"),
	// mer typer perhabs
};

USTRUCT(BlueprintType)
struct FUpgradePreview
{
	GENERATED_BODY()

	//Should be set by the mission type / mission class
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Upgrade Preview") 
	EUpgradeType UpgradeType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Upgrade Preview")
	FString UpgradeDescription; // Example: "This mission rewards a Weapon Augment!"

	FUpgradePreview() : UpgradeType(EUpgradeType::WeaponAugment), UpgradeDescription(TEXT("Default Description")) {}
	
	FUpgradePreview(const EUpgradeType Type, const FString& Desc) : UpgradeType(Type), UpgradeDescription(Desc) {}
};

USTRUCT(BlueprintType)
struct FUpgrade
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) 
	EUpgradeType UpgradeType;

	// Only applicable for WeaponEnhancement & WeaponAugment
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EWeaponType> CompatibleWeaponTypes;

	// Only applicable for WeaponAugment, references the new weapon class
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABaseWeapon> AugmentedWeaponClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStatEnhancementType StatEnhancementType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StatEnhancementValue = 0.0f;

	FUpgrade()
		: UpgradeType(EUpgradeType::WeaponEnhancement), StatEnhancementType(EStatEnhancementType::Damage) {}

	// Parameterized constructor
	FUpgrade(const EUpgradeType Type, const TArray<EWeaponType>& CompatibleWeaponTypes, const TSubclassOf<UObject>& ModClass, const EStatEnhancementType StatType, const float EnhancementValue)
		: UpgradeType(Type), CompatibleWeaponTypes(CompatibleWeaponTypes), AugmentedWeaponClass(ModClass), StatEnhancementType(StatType),  StatEnhancementValue(EnhancementValue) {}
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARSNGUNS_API UMissionUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMissionUpgradeComponent();

	//Funktion som ska fylla upgrade choice widgeten med "lagliga" val från AvailableUpgrades mapen.
	void GetUpgrades();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Upgrade")
	FUpgradePreview UpgradePreview;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Upgrade")
	FUpgrade Upgrade;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Upgrade")
	EUpgradeType UpgradeType = EUpgradeType::WeaponAugment;

	TArray<FUpgrade> Upgrades;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
