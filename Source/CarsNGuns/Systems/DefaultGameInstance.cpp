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
		
		UE_LOG(LogTemp, Log, TEXT("Upgrade Type: %s"), *UEnum::GetDisplayValueAsText(UpgradeType).ToString());
		
		for (const auto& WeaponUpgradePair : WeaponUpgrades)
		{
			EWeaponType WeaponType = WeaponUpgradePair.Key;
			TSharedPtr<FUpgrade> Upgrade = WeaponUpgradePair.Value; // TSharedPtr<FUpgrade> here
			
			if (Upgrade.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("  Weapon Type: %s | Upgrade: %s"), 
					*UEnum::GetDisplayValueAsText(WeaponType).ToString(),
					*Upgrade->DisplayName); // Dereference the shared pointer using '->' to access DisplayName
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Upgrade is null for Weapon Type: %s"), *UEnum::GetDisplayValueAsText(WeaponType).ToString());
			}
		}
	}


	
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
	
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(JsonString);
	
	if (!FJsonSerializer::Deserialize(Reader, JsonArray))
	{
		UE_LOG(LogTemp, Error, TEXT("JSON Deserialization Failed"));
		return;
	}
	
	for (const auto Item : JsonArray)
	{
		const TSharedPtr<FJsonObject> UpgradeObject = Item->AsObject();
		FUpgrade NewUpgrade;

		//Parse UpgradeType
		FString UpgradeTypeStr = UpgradeObject->GetStringField(TEXT("UpgradeType"));
		if (UpgradeTypeStr == "WeaponEnhancement") NewUpgrade.UpgradeType = EUpgradeType::WeaponEnhancement;
		else if (UpgradeTypeStr == "WeaponAugment") NewUpgrade.UpgradeType = EUpgradeType::WeaponAugment;
		else if (UpgradeTypeStr == "VehicleModification") NewUpgrade.UpgradeType = EUpgradeType::VehicleModification;

		//Parse Compatible Weapon Types
		if (UpgradeObject->HasField(TEXT("CompatibleWeaponTypes")))
		{
			TArray<TSharedPtr<FJsonValue>> CompatibleWeapons = UpgradeObject->GetArrayField(TEXT("CompatibleWeaponTypes"));
			for (const auto Weapon : CompatibleWeapons)
			{
				FString WeaponStr = Weapon->AsString();
				if (WeaponStr == "MachineGun") NewUpgrade.CompatibleWeaponTypes.Add(EWeaponType::MachineGun);
				else if (WeaponStr == "LaserRifle") NewUpgrade.CompatibleWeaponTypes.Add(EWeaponType::LaserRifle);
				else if (WeaponStr == "RocketLauncher") NewUpgrade.CompatibleWeaponTypes.Add(EWeaponType::RocketLauncher);
				else if (WeaponStr == "GrenadeLauncher") NewUpgrade.CompatibleWeaponTypes.Add(EWeaponType::GrenadeLauncher);
			}
		}

		//Parse Stat Enhancement Type
		if (UpgradeObject->HasField(TEXT("StatEnhancementType")))
		{
			FString StatEnhancementType = UpgradeObject->GetStringField(TEXT("StatEnhancementType"));
			if (StatEnhancementType == "FireRate") NewUpgrade.StatEnhancementType = EStatEnhancementType::FireRate;
			else if (StatEnhancementType == "Damage") NewUpgrade.StatEnhancementType = EStatEnhancementType::Damage;
			else if (StatEnhancementType == "ReloadSpeed") NewUpgrade.StatEnhancementType = EStatEnhancementType::ReloadSpeed;
		}

		//Parse Stat Enhancement Value
		if (UpgradeObject->HasField(TEXT("StatEnhancementValue")))
		{
			NewUpgrade.StatEnhancementValue = UpgradeObject->GetNumberField(TEXT("StatEnhancementValue"));
		}

		//Parse Stat Enhancement Weapon Upgrade Type
		if (UpgradeObject->HasField(TEXT("UpgradeDamageType")))
		{
			FString UpgradeDamageType = UpgradeObject->GetStringField(TEXT("UpgradeDamageType"));
			if (UpgradeDamageType == "Bullet") NewUpgrade.UpgradeDamageType = EUpgradeDamageType::Bullet;
			else if (UpgradeDamageType == "Explosive") NewUpgrade.UpgradeDamageType = EUpgradeDamageType::Explosive;
			else if (UpgradeDamageType == "Special") NewUpgrade.UpgradeDamageType = EUpgradeDamageType::Special;
		}

		//Parse Augmented Weapon Class
		if (NewUpgrade.UpgradeType == EUpgradeType::WeaponAugment && UpgradeObject->HasField(TEXT("AugmentedWeaponClass")))
		{
			FString AugmentedWeaponClassPath = UpgradeObject->GetStringField(TEXT("AugmentedWeaponClass"));
			if (UClass* LoadedClass = LoadClass<ABaseWeapon>(nullptr, *AugmentedWeaponClassPath)) NewUpgrade.AugmentedWeaponClass = LoadedClass;
			else UE_LOG(LogTemp, Error, TEXT("Failed to load AugmentedWeaponClass: %s"), *AugmentedWeaponClassPath);
		}
		
		//Parse Icon File Path
		if (UpgradeObject->HasField(TEXT("IconFilePath")))
		{
			NewUpgrade.UpgradeIconFilePath = UpgradeObject->GetStringField(TEXT("IconFilePath"));
		}

		//Parse Display Name
		if (UpgradeObject->HasField(TEXT("DisplayName")))
		{
			NewUpgrade.DisplayName = UpgradeObject->GetStringField(TEXT("DisplayName"));
		}

		//Parse Upgrade Description
		if (UpgradeObject->HasField(TEXT("UpgradeDescription")))
		{
			NewUpgrade.UpgradeDescription = UpgradeObject->GetStringField(TEXT("UpgradeDescription"));
		}
		
		//Lägg till upgrades till map
		EUpgradeType UpgradeType = NewUpgrade.UpgradeType;  // The type of the upgrade (EUpgradeType)
		TArray<EWeaponType> CompatibleWeaponTypes = NewUpgrade.CompatibleWeaponTypes;  // Array of compatible weapon types

		//Skapa TSharedPtr<FUpgrade> from NewUpgrade
		TSharedPtr<FUpgrade> SharedUpgrade = MakeShared<FUpgrade>(NewUpgrade); // Construct the TSharedPtr
		
		TMultiMap<EWeaponType, TSharedPtr<FUpgrade>>& WeaponTypeMap = AvailableUpgrades.FindOrAdd(UpgradeType);
		for (const EWeaponType& WeaponType : CompatibleWeaponTypes)
		{
			WeaponTypeMap.Add(WeaponType, SharedUpgrade);
		}

	}
	UE_LOG(LogTemp, Warning, TEXT("Upgrades Initialized Successfully"));
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

