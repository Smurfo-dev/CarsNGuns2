// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	void InitializeValues();
	
};
