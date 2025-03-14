// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DebugMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API UDebugMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "UI")
	class ABasePhysicsVehiclePawn* PlayerReference;

	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* PrimaryWeaponDropDown;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* SecondaryWeaponDropDown;

	UFUNCTION()
	void OnPrimaryWeaponSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnSecondaryWeaponSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void PopulateWeaponDropDowns();
	
};
