// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CarsNGuns/Weapons/BaseWeapon.h"
#include "WeaponSelectionMenu.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMenuClosedSignature);

/**
 * 
 */
UCLASS()
class CARSNGUNS_API UWeaponSelectionMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMenuClosedSignature OnMenuClosed;

	UFUNCTION(BlueprintCallable, Category = "WeaponMenu")
	void CloseMenu();
	
	UPROPERTY(meta = (BindWidget))
	class UButton* WeaponButton1;

	UPROPERTY(meta = (BindWidget))
	UButton* WeaponButton2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WeaponText1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponText2;

	virtual void NativeConstruct() override;
	
	void InitializeMenu(TSubclassOf<ABaseWeapon> Weapon1, TSubclassOf<ABaseWeapon> Weapon2);

private:
	UFUNCTION(BlueprintCallable)
	void OnWeapon1Selected();

	UFUNCTION(BlueprintCallable)
	void OnWeapon2Selected();

	TSubclassOf<ABaseWeapon> SelectedWeapon1;
	TSubclassOf<ABaseWeapon> SelectedWeapon2;

	UPROPERTY()
	class APlayerVehicleBase* PlayerPawn;
	
};
