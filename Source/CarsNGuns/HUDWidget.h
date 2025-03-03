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

	UFUNCTION(BlueprintCallable, Category="Weapons")
	void SetWeaponIcons();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UImage* PrimaryWeaponIcon; 

	UPROPERTY(meta = (BindWidget))
	UImage* SecondaryWeaponIcon; 
	
};
