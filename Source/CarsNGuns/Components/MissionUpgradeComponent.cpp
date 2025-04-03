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
    //Här tar vi 3 upgrades som är "Lämpliga/Lagliga" att appliceras på spelaren från Game Instance Available Upgrades
    if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        ABasePhysicsVehiclePawn* PlayerReference = Cast<ABasePhysicsVehiclePawn>(PlayerPawn);
        if (DefaultGameInstance && PlayerReference)
        {
            Upgrades.Empty();
            UE_LOG(LogTemp, Warning, TEXT("Trying to find upgrades of type: %s"), *UEnum::GetDisplayValueAsText(MissionInfo.UpgradeType).ToString());

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
            
            for (const TPair<EWeaponType, TSharedPtr<FUpgrade>>& UpgradePair : UpgradeMap)
            {
                TSharedPtr<FUpgrade> Upgrade = UpgradePair.Value;
                bool bIsCompatible = false;
                
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
                    FilteredUpgrades.Add(Upgrade);
                }
            }

            int32 NumToSelect = FMath::Min(3, FilteredUpgrades.Num());
            if (NumToSelect > 0)
            {
                FilteredUpgrades.Sort([](const TSharedPtr<FUpgrade>& A, const TSharedPtr<FUpgrade>& B)
                {
                    return FMath::RandBool();
                });
                
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

