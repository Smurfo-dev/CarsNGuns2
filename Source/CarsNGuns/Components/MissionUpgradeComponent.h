// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MissionUpgradeComponent.generated.h"

UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
	WeaponAugment      UMETA(DisplayName="Weapon Augment"), // New weapon abilities
	VehicleAugment     UMETA(DisplayName="Vehicle Augment"), // New vehicle abilities
	WeaponEnhancment    UMETA(DisplayName="Weapon Enhancement"), // Weapon stat increases
	VehicleEnhancement   UMETA(DisplayName="Vehicle Enhancement") // Vehicle stat increases
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Upgrade Preview") 
	EUpgradeType UpgradeType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Upgrade", meta=(EditCondition="UpgradeType == EUpgradeType::WeaponEnhancement || UpgradeType == EUpgradeType::VehicleEnhancement", EditConditionHides))
	float StatBoost = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Upgrade", meta=(EditCondition="UpgradeType == EUpgradeType::WeaponAugment || UpgradeType == EUpgradeType::VehicleAugment", EditConditionHides))
	TSubclassOf<UObject> ModifierClass;

	FUpgrade()
		: UpgradeType(EUpgradeType::WeaponAugment) {}

	// Parameterized constructor
	FUpgrade(const EUpgradeType Type, const float Boost, const TSubclassOf<UObject>& ModClass)
		: UpgradeType(Type), StatBoost(Boost), ModifierClass(ModClass) {}
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARSNGUNS_API UMissionUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMissionUpgradeComponent();

	void InitializeUpgrades();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Upgrade")
	FUpgradePreview UpgradePreview;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Upgrade")
	FUpgrade Upgrade;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Upgrade")
	EUpgradeType UpgradeType = EUpgradeType::VehicleAugment;

	TArray<FUpgrade> Upgrades;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
