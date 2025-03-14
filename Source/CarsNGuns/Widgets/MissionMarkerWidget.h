// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MissionMarkerWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API UMissionMarkerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "UI")
	class ABasePhysicsVehiclePawn* PlayerReference;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "UI")
	class ABaseMission* MissionReference;

	UPROPERTY(meta = (BindWidget))
	class UImage* MarkerImage;

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DistanceText;
	
};
