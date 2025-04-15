// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
#include "CarsNGuns/Components/MissionUpgradeComponent.h"

void UDefaultGameInstance::Init()
{
	Super::Init();

	InitializeUpgrades();

	//Log all upgrades available
	for (const auto& UpgradePair : AvailableUpgrades)
	{
		EUpgradeType UpgradeType = UpgradePair.Key;
		const TMultiMap<EWeaponType, TSharedPtr<FUpgrade>>& WeaponUpgrades = UpgradePair.Value; // Note the TSharedPtr<FUpgrade>
    
		// Log a separator for clarity
		UE_LOG(LogTemp, Log, TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
		UE_LOG(LogTemp, Log, TEXT("Upgrade Type: %s"), *UEnum::GetDisplayValueAsText(UpgradeType).ToString());
    
		for (const auto& WeaponUpgradePair : WeaponUpgrades)
		{
			EWeaponType WeaponType = WeaponUpgradePair.Key;
			TSharedPtr<FUpgrade> Upgrade = WeaponUpgradePair.Value; // TSharedPtr<FUpgrade> here
        
			if (Upgrade.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("  Weapon Type: %s | DisplayName: %s"), 
					*UEnum::GetDisplayValueAsText(WeaponType).ToString(),
					*Upgrade->DisplayName); // Dereference the shared pointer using '->' to access DisplayName
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Upgrade is null for Weapon Type: %s"), *UEnum::GetDisplayValueAsText(WeaponType).ToString());
			}
		}
	}

	// Add a separator at the end for clarity
	UE_LOG(LogTemp, Log, TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));

}

void UDefaultGameInstance::InitializeUpgrades()
{
	FString FilePath = FPaths::ProjectContentDir() + "Upgrades/upgrades.json";
	FString JsonString;

	if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load JSON file!"));
		return;
	}

	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(JsonString);
	TSharedPtr<FJsonObject> RootObject;

	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to deserialize upgrade JSON."));
		return;
	}

	// Go through each upgrade category
	for (const auto& UpgradeCategory : RootObject->Values)
	{
		const FString& CategoryName = UpgradeCategory.Key;
		const TArray<TSharedPtr<FJsonValue>>* UpgradeArray;

		if (RootObject->TryGetArrayField(CategoryName, UpgradeArray))
		{
			for (const auto& Value : *UpgradeArray)
			{
				const TSharedPtr<FJsonObject> UpgradeObject = Value->AsObject();
				if (!UpgradeObject.IsValid()) continue;

				FUpgrade NewUpgrade;
				ParseUpgradeFromJson(UpgradeObject, NewUpgrade);

				// Add upgrade to map
				EUpgradeType UpgradeType = NewUpgrade.UpgradeType;
				TArray<EWeaponType> CompatibleTypes = NewUpgrade.CompatibleWeaponTypes;

				TSharedPtr<FUpgrade> SharedUpgrade = MakeShared<FUpgrade>(NewUpgrade);
				TMultiMap<EWeaponType, TSharedPtr<FUpgrade>>& WeaponMap = AvailableUpgrades.FindOrAdd(UpgradeType);

				for (const EWeaponType& WeaponType : CompatibleTypes)
				{
					WeaponMap.Add(WeaponType, SharedUpgrade);
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Upgrades Initialized Successfully"));
}


void UDefaultGameInstance::ParseUpgradeFromJson(const TSharedPtr<FJsonObject>& UpgradeObject, FUpgrade& OutUpgrade)
{
	// Parse Upgrade Type
	FString UpgradeTypeStr = UpgradeObject->GetStringField(TEXT("UpgradeType"));
	if (UpgradeTypeStr == "WeaponEnhancement") OutUpgrade.UpgradeType = EUpgradeType::WeaponEnhancement;
	else if (UpgradeTypeStr == "WeaponAugment") OutUpgrade.UpgradeType = EUpgradeType::WeaponAugment;
	else if (UpgradeTypeStr == "VehicleModification") OutUpgrade.UpgradeType = EUpgradeType::VehicleModification;
	else if (UpgradeTypeStr == "WheelUpgrade") OutUpgrade.UpgradeType = EUpgradeType::WheelUpgrade;

	// Compatible Weapon Types
	if (UpgradeObject->HasField(TEXT("CompatibleWeaponTypes")))
	{
		for (const auto& Weapon : UpgradeObject->GetArrayField(TEXT("CompatibleWeaponTypes")))
		{
			FString WeaponStr = Weapon->AsString();
			if (WeaponStr == "None") OutUpgrade.CompatibleWeaponTypes.Add(EWeaponType::None);
			else
			{
				if (WeaponStr == "MachineGun") OutUpgrade.CompatibleWeaponTypes.Add(EWeaponType::MachineGun);
				else if (WeaponStr == "LaserRifle") OutUpgrade.CompatibleWeaponTypes.Add(EWeaponType::LaserRifle);
				else if (WeaponStr == "RocketLauncher") OutUpgrade.CompatibleWeaponTypes.Add(EWeaponType::RocketLauncher);
				else if (WeaponStr == "GrenadeLauncher") OutUpgrade.CompatibleWeaponTypes.Add(EWeaponType::GrenadeLauncher);
			}
		}
	}

	// Stat Enhancement Type
	if (UpgradeObject->HasField(TEXT("StatEnhancementType")))
	{
		FString StatTypeStr = UpgradeObject->GetStringField(TEXT("StatEnhancementType"));
		if (StatTypeStr == "FireRate") OutUpgrade.StatEnhancementType = EStatEnhancementType::FireRate;
		else if (StatTypeStr == "Damage") OutUpgrade.StatEnhancementType = EStatEnhancementType::Damage;
		else if (StatTypeStr == "ReloadSpeed") OutUpgrade.StatEnhancementType = EStatEnhancementType::ReloadSpeed;
	}

	// Stat Value
	if (UpgradeObject->HasField(TEXT("StatEnhancementValue")))
	{
		OutUpgrade.StatEnhancementValue = UpgradeObject->GetNumberField(TEXT("StatEnhancementValue"));
	}

	// Damage Type
	if (UpgradeObject->HasField(TEXT("UpgradeDamageType")))
	{
		FString DmgTypeStr = UpgradeObject->GetStringField(TEXT("UpgradeDamageType"));
		if (DmgTypeStr == "Bullet") OutUpgrade.DamageType = EDamageType::Bullet;
		else if (DmgTypeStr == "Explosive") OutUpgrade.DamageType = EDamageType::Explosive;
		else if (DmgTypeStr == "Special") OutUpgrade.DamageType = EDamageType::Special;
	}

	// Augmented Weapon Class
	if (OutUpgrade.UpgradeType == EUpgradeType::WeaponAugment && UpgradeObject->HasField(TEXT("AugmentedWeaponClass")))
	{
		FString ClassPath = UpgradeObject->GetStringField(TEXT("AugmentedWeaponClass"));
		if (UClass* LoadedClass = LoadClass<ABaseWeapon>(nullptr, *ClassPath))
			OutUpgrade.AugmentedWeaponClass = LoadedClass;
		else
			UE_LOG(LogTemp, Error, TEXT("Failed to load AugmentedWeaponClass: %s"), *ClassPath);
	}

	// Wheel Config
	if (UpgradeObject->HasField(TEXT("WheelConfigAsset")))
	{
		FString WheelConfigPath = UpgradeObject->GetStringField(TEXT("WheelConfigAsset"));
		if (UWheelConfigData* LoadedAsset = LoadObject<UWheelConfigData>(nullptr, *WheelConfigPath))
			OutUpgrade.WheelConfigAsset = LoadedAsset;
		else
			UE_LOG(LogTemp, Error, TEXT("Failed to load WheelConfigAsset: %s"), *WheelConfigPath);
	}

	// Icon
	if (UpgradeObject->HasField(TEXT("IconFilePath")))
		OutUpgrade.UpgradeIconFilePath = UpgradeObject->GetStringField(TEXT("IconFilePath"));

	// Display Name
	if (UpgradeObject->HasField(TEXT("DisplayName")))
		OutUpgrade.DisplayName = UpgradeObject->GetStringField(TEXT("DisplayName"));

	// Description
	if (UpgradeObject->HasField(TEXT("UpgradeDescription")))
		OutUpgrade.UpgradeDescription = UpgradeObject->GetStringField(TEXT("UpgradeDescription"));
}


void UDefaultGameInstance::StartTimer()
{
	if (bIsPaused)
	{
		bIsPaused = false;
		StartTime = GetWorld()->GetTimeSeconds();
		PausedElapsedTime = 0.0f;
	}
}

void UDefaultGameInstance::PauseTimer()
{
	if (!bIsPaused)
	{
		PausedElapsedTime = GetElapsedTime();
		bIsPaused = true;
	}
}

void UDefaultGameInstance::ResumeTimer()
{
	if (bIsPaused)
	{
		bIsPaused = false;
		StartTime = GetWorld()->GetTimeSeconds() - PausedElapsedTime;
	}
}

void UDefaultGameInstance::StopTimer()
{
	bIsPaused = true;
}

float UDefaultGameInstance::GetElapsedTime() const
{
	if (!bIsPaused)
	{
		return GetWorld()->GetTimeSeconds() - StartTime;
	}
	return PausedElapsedTime;
}

