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
    // Create 3 upgrades that are eligible to be applied based on EUpgradeType and the player's current upgrades/equipped weapons
    if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        ABasePhysicsVehiclePawn* PlayerReference = Cast<ABasePhysicsVehiclePawn>(PlayerPawn);
        if (DefaultGameInstance && PlayerReference)
        {
            Upgrades.Empty();
            UE_LOG(LogTemp, Error, TEXT("Trying to find upgrades of type: %s"), *UEnum::GetDisplayValueAsText(MissionInfo.UpgradeType).ToString());

            if (!DefaultGameInstance->GetAvailableUpgrades().Contains(MissionInfo.UpgradeType))
            {
                UE_LOG(LogTemp, Error, TEXT("No upgrades found for given upgrade type: %d"), MissionInfo.UpgradeType);
                return;
            }

            // Get the UpgradeMap for the given UpgradeType
            const TMultiMap<EWeaponType, TSharedPtr<FUpgrade>>& UpgradeMap = DefaultGameInstance->GetAvailableUpgrades()[MissionInfo.UpgradeType];

            // Array to hold filtered upgrades as shared pointers
            TArray<TSharedPtr<FUpgrade>> FilteredUpgrades;

            auto IsUpgradeAlreadyAdded = [&](const TSharedPtr<FUpgrade>& Upgrade) -> bool
            {
                return FilteredUpgrades.ContainsByPredicate([&](const TSharedPtr<FUpgrade>& ExistingUpgrade)
                {
                    return ExistingUpgrade->DisplayName == Upgrade->DisplayName; // Display name has to be unique right now
                });
            };

            // Iterate through the UpgradeMap
            for (const TPair<EWeaponType, TSharedPtr<FUpgrade>>& UpgradePair : UpgradeMap)
            {
                TSharedPtr<FUpgrade> Upgrade = UpgradePair.Value; // Now using TSharedPtr<FUpgrade>
                bool bIsCompatible = false;

                // Check if the upgrade is compatible with the player's primary or secondary weapon
                if (PlayerReference->GetPrimaryWeapon() && UpgradePair.Key == PlayerReference->GetPrimaryWeapon()->GetWeaponType() && !IsUpgradeAlreadyAdded(Upgrade))
                {
                    bIsCompatible = true;
                }

                if (!bIsCompatible && PlayerReference->GetSecondaryWeapon() && UpgradePair.Key == PlayerReference->GetSecondaryWeapon()->GetWeaponType() && !IsUpgradeAlreadyAdded(Upgrade))
                {
                    bIsCompatible = true;
                }

                if (bIsCompatible)
                {
                    FilteredUpgrades.Add(Upgrade); // Add the shared pointer to the list of filtered upgrades
                }
            }

            int32 NumToSelect = FMath::Min(3, FilteredUpgrades.Num());
            if (NumToSelect > 0)
            {
                // Sort the filtered upgrades randomly
                FilteredUpgrades.Sort([](const TSharedPtr<FUpgrade>& A, const TSharedPtr<FUpgrade>& B)
                {
                    return FMath::RandBool();
                });

                // Append the selected upgrades to the Upgrades array
                for (int32 i = 0; i < NumToSelect; ++i)
                {
                    Upgrades.Add(*FilteredUpgrades[i]);
                }
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

