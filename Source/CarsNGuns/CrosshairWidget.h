// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// This is the reference variable you want to set in C++
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "UI")
	class ABasePhysicsVehiclePawn* PlayerReference;
	
};
