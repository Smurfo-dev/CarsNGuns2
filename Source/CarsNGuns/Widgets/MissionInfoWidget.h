// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CarsNGuns/Components/MissionUpgradeComponent.h"
#include "MissionInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API UMissionInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void InitializeValues(const FMissionInfo& MissionInfo);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MissionName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MissionType;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UpgradeType;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UpgradeDescription;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PressToStart;
	
};
