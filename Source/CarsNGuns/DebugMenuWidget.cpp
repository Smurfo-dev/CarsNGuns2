// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugMenuWidget.h"
#include "BaseWeapon.h"
#include "DefaultGameInstance.h"
#include "Components/ComboBoxString.h"

void UDebugMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PrimaryWeaponDropDown) PrimaryWeaponDropDown->OnSelectionChanged.AddDynamic(this, &UDebugMenuWidget::OnPrimaryWeaponSelected);
	if (SecondaryWeaponDropDown) SecondaryWeaponDropDown->OnSelectionChanged.AddDynamic(this, &UDebugMenuWidget::OnSecondaryWeaponSelected);
}

void UDebugMenuWidget::OnPrimaryWeaponSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(GetGameInstance()))
	{
		if (GameInstance->AvailableWeapons.IsValidIndex(PrimaryWeaponDropDown->GetSelectedIndex()))
		{
			PlayerReference->AttachPrimaryWeaponToVehicle(GameInstance->AvailableWeapons[PrimaryWeaponDropDown->GetSelectedIndex()]);
		}
	}
		
			
}

void UDebugMenuWidget::OnSecondaryWeaponSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(GetGameInstance()))
	{
		if (GameInstance->AvailableWeapons.IsValidIndex(SecondaryWeaponDropDown->GetSelectedIndex()))
		{
			PlayerReference->AttachSecondaryWeaponToVehicle(GameInstance->AvailableWeapons[SecondaryWeaponDropDown->GetSelectedIndex()]);
		}
	}
}

void UDebugMenuWidget::PopulateWeaponDropDowns()
{
	if (UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(GetGameInstance()))
	{
		PrimaryWeaponDropDown->ClearOptions();
		SecondaryWeaponDropDown->ClearOptions();

		TArray<FString> WeaponNames;

		for (auto& Weapon : GameInstance->AvailableWeapons)
		{
			ABaseWeapon* WeaponInstance = Weapon->GetDefaultObject<ABaseWeapon>();
			if (WeaponInstance) WeaponNames.Add(WeaponInstance->GetWeaponID());
		}

		for (const FString& WeaponName : WeaponNames)
		{
			// Add options to both combo boxes
			PrimaryWeaponDropDown->AddOption(WeaponName);
			SecondaryWeaponDropDown->AddOption(WeaponName);
		}

		SetInitialWeaponSelection(GameInstance, WeaponNames);
		
	}
	
}

void UDebugMenuWidget::SetInitialWeaponSelection(UDefaultGameInstance* GameInstance, const TArray<FString>& WeaponNames)
{
	if (PlayerReference)
	{
		if (ABaseWeapon* PrimaryWeapon = PlayerReference->GetPrimaryWeapon())
		{
			int32 PrimaryWeaponIndex = WeaponNames.IndexOfByKey(PrimaryWeapon->GetWeaponID());
			if (PrimaryWeaponIndex != INDEX_NONE) PrimaryWeaponDropDown->SetSelectedIndex(PrimaryWeaponIndex);
		}

		if (ABaseWeapon* SecondaryWeapon = PlayerReference->GetSecondaryWeapon())
		{
			int32 SecondaryWeaponIndex = WeaponNames.IndexOfByKey(SecondaryWeapon->GetWeaponID());
			if (SecondaryWeaponIndex != INDEX_NONE) SecondaryWeaponDropDown->SetSelectedIndex(SecondaryWeaponIndex);
		}
	}
}
