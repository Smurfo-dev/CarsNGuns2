// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class ABasePhysicsVehiclePawn;
/**
 * 
 */
UCLASS()
class CARSNGUNS_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// This is the reference variable you want to set in C++
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "UI")
	ABasePhysicsVehiclePawn* PlayerReference;
	
};
