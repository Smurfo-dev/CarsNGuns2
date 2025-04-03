// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarsNGuns/Weapons/BaseWeapon.h"
#include "UpgradeHandlerComponent.generated.h"

UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
	WeaponAugment      UMETA(DisplayName="Weapon Augment"), // New Weapon Mechanics
	WeaponEnhancement    UMETA(DisplayName="Weapon Enhancement"), // Weapon stat boosts
	VehicleModification   UMETA(DisplayName="Vehicle Modification") // Vehicle Modifications (stats and perhaps mechanics, visual modifications possibly)
};

USTRUCT(BlueprintType)
struct FUpgrade
{
	GENERATED_BODY()
	
	EUpgradeType UpgradeType;

	// Only applicable for WeaponEnhancement & WeaponAugment
	TArray<EWeaponType> CompatibleWeaponTypes;

	// Only applicable for WeaponAugment, references the new weapon class
	TSubclassOf<ABaseWeapon> AugmentedWeaponClass;
	
	EStatEnhancementType StatEnhancementType;
	
	float StatEnhancementValue = 0.0f;

	EUpgradeDamageType UpgradeDamageType;
	
	FString UpgradeIconFilePath;
	
	FString DisplayName;
	
	FString UpgradeDescription;

	FUpgrade()
		: UpgradeType(EUpgradeType::WeaponEnhancement), StatEnhancementType(EStatEnhancementType::Damage), UpgradeDamageType(EUpgradeDamageType::Bullet) {}

	// Parameterized constructor
	FUpgrade(const EUpgradeType Type, const TArray<EWeaponType>& CompatibleWeaponTypes, const TSubclassOf<UObject>& ModClass, const EStatEnhancementType StatType, const float EnhancementValue, const EUpgradeDamageType UpgradeDamageType,
		const FString& IconFilePath, const FString& DisplayName, const FString& UpgradeDescription)
		: UpgradeType(Type), CompatibleWeaponTypes(CompatibleWeaponTypes), AugmentedWeaponClass(ModClass), StatEnhancementType(StatType), StatEnhancementValue(EnhancementValue), UpgradeDamageType(UpgradeDamageType),
			UpgradeIconFilePath(IconFilePath), DisplayName(DisplayName), UpgradeDescription(UpgradeDescription) {}
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARSNGUNS_API UUpgradeHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

	TArray<FUpgrade> EquippedUpgrades;

	void ApplyEnhancement(const FUpgrade& Upgrade);
	void ApplyAugment(const FUpgrade& Upgrade);
	void ApplyModification(const FUpgrade& Upgrade);

public:	
	// Sets default values for this component's properties
	UUpgradeHandlerComponent();

	void AddUpgrade(const FUpgrade& Upgrade);
	

	UPROPERTY()
	ABasePhysicsVehiclePawn* OwnerReference;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
};
