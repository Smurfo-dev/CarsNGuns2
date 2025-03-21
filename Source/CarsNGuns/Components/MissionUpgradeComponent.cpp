// Fill out your copyright notice in the Description page of Project Settings.


#include "CarsNGuns/Components/MissionUpgradeComponent.h"

#include "CarsNGuns/Systems/DefaultGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UMissionUpgradeComponent::UMissionUpgradeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UMissionUpgradeComponent::GetUpgrades()
{
	//Här skapar vi 3 upgrades som är lagliga att appliceras utifrån EUpgradeType Och vad spelaren redan har för upgrades/equippade vapen
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		ABasePhysicsVehiclePawn* PlayerReference = Cast<ABasePhysicsVehiclePawn>(PlayerPawn);
		if (DefaultGameInstance && PlayerReference)
		{
			Upgrades.Empty();
			if (!DefaultGameInstance->GetAvailableUpgrades().Contains(UpgradeType))
			{
				UE_LOG(LogTemp, Error, TEXT("No upgrades found for given upgrade type: %d"), UpgradeType)
				return;
			}
			const TMultiMap<EWeaponType, FUpgrade>& UpgradeMap = DefaultGameInstance->GetAvailableUpgrades()[UpgradeType];

			// Array to hold filtered upgrades
			TArray<FUpgrade> FilteredUpgrades;

			auto IsUpgradeAlreadyAdded = [&](const FUpgrade& Upgrade) -> bool
			{
				return FilteredUpgrades.ContainsByPredicate([&](const FUpgrade& ExistingUpgrade)
				{
					return ExistingUpgrade.DisplayName == Upgrade.DisplayName; // Display name has to be unique right now
				});
			};

			for (const TPair<EWeaponType, FUpgrade>& UpgradePair : UpgradeMap)
			{
				const FUpgrade& Upgrade = UpgradePair.Value;
				bool bIsCompatible = false;

				if (PlayerReference->GetPrimaryWeapon() && Upgrade.CompatibleWeaponTypes.Contains(PlayerReference->GetPrimaryWeapon()->GetWeaponType()) && !IsUpgradeAlreadyAdded(Upgrade))
				{
					bIsCompatible = true;
				}

				if (!bIsCompatible && PlayerReference->GetSecondaryWeapon() && Upgrade.CompatibleWeaponTypes.Contains(PlayerReference->GetSecondaryWeapon()->GetWeaponType()) && !IsUpgradeAlreadyAdded(Upgrade))
				{
					bIsCompatible = true;
				}
				
				if (bIsCompatible) FilteredUpgrades.Add(Upgrade);
			}
			
			int32 NumToSelect = FMath::Min(3, FilteredUpgrades.Num());
			if (NumToSelect > 0)
			{
				FilteredUpgrades.Sort([](const FUpgrade& A, const FUpgrade& B)
				{
					return FMath::RandBool();
				});

				Upgrades.Append(FilteredUpgrades.GetData(), NumToSelect);
			}
		}
		
	}
	
	
}


// Called when the game starts
void UMissionUpgradeComponent::BeginPlay()
{
	Super::BeginPlay();

	DefaultGameInstance = GetWorld()->GetGameInstance<UDefaultGameInstance>();
	
}


// Called every frame
void UMissionUpgradeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

