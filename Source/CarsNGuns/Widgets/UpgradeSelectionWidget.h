// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CarsNGuns/Components/MissionUpgradeComponent.h"
#include "CarsNGuns/Missions/BaseMission.h"
#include "UpgradeSelectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API UUpgradeSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	const TMap<EUpgradeRarity, FLinearColor> RarityColorMap = {
	{ EUpgradeRarity::Common, FLinearColor(1, 1, 1)},
	{ EUpgradeRarity::Rare, FLinearColor(0, 0.1, 1)},
	{ EUpgradeRarity::Epic, FLinearColor(0.5, 0, 1)},
	{ EUpgradeRarity::Legendary, FLinearColor(1, 0.5, 0)},
	{ EUpgradeRarity::Mythic, FLinearColor(1, 0, 0)},
	};
	
	UPROPERTY(meta = (BindWidget))
	class UImage* Option1Image;

	UPROPERTY(meta = (BindWidget))
	UImage* Option2Image;

	UPROPERTY(meta = (BindWidget))
	UImage* Option3Image;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Option1DisplayName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Option2DisplayName;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Option3DisplayName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Option1Description;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Option2Description;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Option3Description;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Option1Rarity;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Option2Rarity;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Option3Rarity;

	UPROPERTY(meta = (BindWidget))
	class UButton* Option1Button;

	UPROPERTY(meta = (BindWidget))
	UButton* Option2Button;

	UPROPERTY(meta = (BindWidget))
	UButton* Option3Button;

	void InitializeValues(const TArray<FUpgrade>& Upgrades, AMyPlayerController* PlayerControllerRef, ABaseMission* MissionRef);

protected:
	UFUNCTION()
	void OnOption1Clicked();

	UFUNCTION()
	void OnOption2Clicked();

	UFUNCTION()
	void OnOption3Clicked();

	virtual void NativeConstruct() override;

	UPROPERTY()
	AMyPlayerController* PlayerController;

	UPROPERTY()
	ABaseMission* MissionReference;

private:
	void SetImageFromFile(const FString& FilePath, UImage* Image);

	UTexture2D* LoadTextureFromFile(const FString& FilePath);

	void DisableWidget();
	
};
