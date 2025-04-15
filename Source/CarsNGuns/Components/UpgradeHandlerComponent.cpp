// Fill out your copyright notice in the Description page of Project Settings.


#include "CarsNGuns/Components/UpgradeHandlerComponent.h"
#include "CarsNGuns/Vehicles/BasePhysicsVehiclePawn.h"

// Sets default values for this component's properties
UUpgradeHandlerComponent::UUpgradeHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}


// Called when the game starts
void UUpgradeHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
	
}

void UUpgradeHandlerComponent::AddUpgrade(const FUpgrade& Upgrade)
{
	EquippedUpgrades.Add(Upgrade); // Hur hanterar vi stackade upgrades? 2 Fire rate buffs?
	switch (Upgrade.UpgradeType)
	{
	case EUpgradeType::WeaponEnhancement:
		ApplyEnhancement(Upgrade);
		break;

	case EUpgradeType::WeaponAugment:
		ApplyAugment(Upgrade);
		break;

	case EUpgradeType::VehicleModification:
		ApplyModification(Upgrade);
		break;
	case EUpgradeType::WheelUpgrade:
		ApplyWheelUpgrade(Upgrade);
		break;
	}
}

void UUpgradeHandlerComponent::ApplyEnhancement(const FUpgrade& Upgrade)
{
	switch (Upgrade.StatEnhancementType)
	{
	case EStatEnhancementType::Damage:
		*StatConfig.DamageMultipliers.Find(Upgrade.DamageType) += 1.0f + Upgrade.StatEnhancementValue / 100.0f;
		break;

	case EStatEnhancementType::FireRate:
		*StatConfig.FireRateMultipliers.Find(Upgrade.DamageType) += 1.0f + Upgrade.StatEnhancementValue / 100.0f;
		break;

	case EStatEnhancementType::ReloadSpeed:
		*StatConfig.RechargeMultipliers.Find(Upgrade.DamageType) += 1.0f + Upgrade.StatEnhancementValue / 100.0f;
		break;
	}
}

void UUpgradeHandlerComponent::ApplyAugment(const FUpgrade& Upgrade)
{
	if (Upgrade.AugmentedWeaponClass) UE_LOG(LogTemp, Warning, TEXT("Augment Class Valid"))
	ABaseWeapon* DefaultUpgradeClass = Upgrade.AugmentedWeaponClass.GetDefaultObject();
	if (OwnerReference->GetPrimaryWeapon() && OwnerReference->GetPrimaryWeapon()->GetWeaponType() == DefaultUpgradeClass->GetWeaponType())
	{
		OwnerReference->AttachPrimaryWeaponToVehicle(Upgrade.AugmentedWeaponClass);
	}
	else if (OwnerReference->GetSecondaryWeapon() && OwnerReference->GetSecondaryWeapon()->GetWeaponType() == DefaultUpgradeClass->GetWeaponType())
	{
		OwnerReference->AttachSecondaryWeaponToVehicle(Upgrade.AugmentedWeaponClass);
	}
}

void UUpgradeHandlerComponent::ApplyModification(const FUpgrade& Upgrade)
{
	
}

void UUpgradeHandlerComponent::ApplyWheelUpgrade(const FUpgrade& Upgrade)
{
	if (Upgrade.WheelConfigAsset.IsValid())
	{
		const UWheelConfigData* WheelConfig = Upgrade.WheelConfigAsset.Get();
		OwnerReference->InitWheelConfig(WheelConfig);
	}
}



