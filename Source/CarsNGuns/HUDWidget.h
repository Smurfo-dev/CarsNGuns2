// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "DefaultGameInstance.h"
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

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "UI")
	UDefaultGameInstance* DefaultGameInstance;

	UFUNCTION(BlueprintCallable, Category="Weapons")
	void SetWeaponIcons();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UImage* PrimaryWeaponIcon; 

	UPROPERTY(meta = (BindWidget))
	UImage* SecondaryWeaponIcon;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PrimaryProgress;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SecondaryProgress;

	UPROPERTY(meta = (BindWidget))
	UImage* PrimaryCircularProgress;

	UPROPERTY(meta = (BindWidget))
	UImage* SecondaryCircularProgress;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WorldTimer;

	UPROPERTY(EditDefaultsOnly, Category=Materials)
	UMaterialInstanceDynamic* PrimaryMaterialInstance;
	
	UPROPERTY(EditDefaultsOnly, Category=Materials)
	UMaterialInstanceDynamic* SecondaryMaterialInstance;

private:
	UPROPERTY()
	FString EquippedPrimaryID = "Default";

	UPROPERTY()
	FString EquippedSecondaryID = "Default";

	float CalculateProgress(FString WeaponID, int32 WeaponIndex); //index 0 or 1 for primary or secondary
	
	void SetBarVisibilityPrimary(FString EquipID);

	void SetBarVisibilitySecondary(FString EquipID);

	void SetupCircularProgressBars();

	void UpdateCircularProgressBar(UMaterialInstanceDynamic* MaterialInstance, float ProgressValue);
	
};
