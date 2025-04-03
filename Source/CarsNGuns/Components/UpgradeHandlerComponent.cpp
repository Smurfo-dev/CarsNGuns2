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
	EquippedUpgrades.Add(Upgrade);
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
		
	}

}

void UUpgradeHandlerComponent::ApplyEnhancement(const FUpgrade& Upgrade)
{
	switch (Upgrade.StatEnhancementType)
	{
	case EStatEnhancementType::Damage:
		if (OwnerReference->GetPrimaryWeapon() && OwnerReference->GetPrimaryWeapon()->GetUpgradeDamageType() == Upgrade.UpgradeDamageType)
		{
			OwnerReference->GetPrimaryWeapon()->SetDamage(Upgrade.StatEnhancementValue);
		}
		if (OwnerReference->GetSecondaryWeapon() && OwnerReference->GetSecondaryWeapon()->GetUpgradeDamageType() == Upgrade.UpgradeDamageType)
		{
			OwnerReference->GetSecondaryWeapon()->SetDamage(Upgrade.StatEnhancementValue);
		}
		break;

	case EStatEnhancementType::FireRate:
		if (OwnerReference->GetPrimaryWeapon() && OwnerReference->GetPrimaryWeapon()->GetUpgradeDamageType() == Upgrade.UpgradeDamageType)
		{
			OwnerReference->GetPrimaryWeapon()->SetFireRate(Upgrade.StatEnhancementValue);
		}
		if (OwnerReference->GetSecondaryWeapon() && OwnerReference->GetSecondaryWeapon()->GetUpgradeDamageType() == Upgrade.UpgradeDamageType)
		{
			OwnerReference->GetSecondaryWeapon()->SetFireRate(Upgrade.StatEnhancementValue);
		}
		break;

	case EStatEnhancementType::ReloadSpeed:
		if (OwnerReference->GetPrimaryWeapon() && OwnerReference->GetPrimaryWeapon()->GetUpgradeDamageType() == Upgrade.UpgradeDamageType)
		{
			OwnerReference->GetPrimaryWeapon()->SetReloadSpeed(Upgrade.StatEnhancementValue);
		}
		if (OwnerReference->GetSecondaryWeapon() && OwnerReference->GetSecondaryWeapon()->GetUpgradeDamageType() == Upgrade.UpgradeDamageType)
		{
			OwnerReference->GetSecondaryWeapon()->SetReloadSpeed(Upgrade.StatEnhancementValue);
		}
		break;
	}
}

void UUpgradeHandlerComponent::ApplyAugment(const FUpgrade& Upgrade)
{
	
}

void UUpgradeHandlerComponent::ApplyModification(const FUpgrade& Upgrade)
{
	
}


