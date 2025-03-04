// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "BaseWeapon.h"
#include "BasePhysicsVehiclePawn.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PlayerReference)
	{
		if (PlayerReference->GetPrimaryWeapon())
		{
			if (EquippedPrimaryID == "Machine Gun" || EquippedPrimaryID == "Laser Rifle")
			{
				PrimaryProgress->SetPercent(CalculateProgress(EquippedPrimaryID, 0));
			}
			else if (EquippedPrimaryID == "Rocket Launcher" || EquippedPrimaryID == "Grenade Launcher")
			{
				UpdateCircularProgressBar(PrimaryMaterialInstance, CalculateProgress(EquippedPrimaryID, 0));
			}
		
		}
		if (PlayerReference->GetSecondaryWeapon())
		{
			if (EquippedSecondaryID == "Machine Gun" || EquippedSecondaryID == "Laser Rifle")
			{
				SecondaryProgress->SetPercent(CalculateProgress(EquippedSecondaryID, 1));
			}
			else if (EquippedSecondaryID == "Rocket Launcher" || EquippedSecondaryID == "Grenade Launcher")
			{
				UpdateCircularProgressBar(SecondaryMaterialInstance, CalculateProgress(EquippedSecondaryID, 1));
			}
		}
	}
}

float UHUDWidget::CalculateProgress(FString WeaponID, int32 WeaponIndex)
{
	if (WeaponIndex == 0) return PlayerReference->GetPrimaryWeapon()->GetResourceBarValue();
	return PlayerReference->GetSecondaryWeapon()->GetResourceBarValue();
	
}

void UHUDWidget::SetBarVisibilityPrimary(FString EquipID)
{
	if (EquipID == "Machine Gun" || EquipID == "Laser Rifle")
	{
		PrimaryCircularProgress->SetVisibility(ESlateVisibility::Hidden);
		PrimaryProgress->SetVisibility(ESlateVisibility::Visible);
	}
	else if (EquipID == "Rocket Launcher" || EquipID == "Grenade Launcher")
	{
		PrimaryProgress->SetVisibility(ESlateVisibility::Hidden);
		PrimaryCircularProgress->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		PrimaryProgress->SetVisibility(ESlateVisibility::Hidden);
		PrimaryCircularProgress->SetVisibility(ESlateVisibility::Hidden);
	}
	
}

void UHUDWidget::SetBarVisibilitySecondary(FString EquipID)
{
	if (EquipID == "Machine Gun" || EquipID == "Laser Rifle")
	{
		SecondaryCircularProgress->SetVisibility(ESlateVisibility::Hidden);
		SecondaryProgress->SetVisibility(ESlateVisibility::Visible);
	}
	else if (EquipID == "Rocket Launcher" || EquipID == "Grenade Launcher")
	{
		SecondaryProgress->SetVisibility(ESlateVisibility::Hidden);
		SecondaryCircularProgress->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		
		SecondaryProgress->SetVisibility(ESlateVisibility::Hidden);
		SecondaryCircularProgress->SetVisibility(ESlateVisibility::Hidden);
	}
}


void UHUDWidget::SetWeaponIcons()
{
	if (PlayerReference->GetPrimaryWeapon() && PlayerReference->GetPrimaryWeapon()->GetWeaponIcon())
	{
		PrimaryWeaponIcon->SetBrushFromTexture(PlayerReference->GetPrimaryWeapon()->GetWeaponIcon());
		PrimaryWeaponIcon->SetVisibility(ESlateVisibility::Visible);
	}
	
	if (PlayerReference->GetSecondaryWeapon() && PlayerReference->GetSecondaryWeapon()->GetWeaponIcon())
	{
		SecondaryWeaponIcon->SetBrushFromTexture(PlayerReference->GetSecondaryWeapon()->GetWeaponIcon());
		SecondaryWeaponIcon->SetVisibility(ESlateVisibility::Visible);
	}

	if (PlayerReference->GetPrimaryWeapon()) EquippedPrimaryID = PlayerReference->GetPrimaryWeapon()->GetWeaponID();
	if (PlayerReference->GetSecondaryWeapon()) EquippedSecondaryID = PlayerReference->GetSecondaryWeapon()->GetWeaponID();

	SetBarVisibilityPrimary(EquippedPrimaryID);
	SetBarVisibilitySecondary(EquippedSecondaryID);

	SetupCircularProgressBars();
}

void UHUDWidget::SetupCircularProgressBars()
{
	UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/Materials/UI/M_CircularProgress.M_CircularProgress'"));
	if (BaseMaterial)
	{
		if (PrimaryCircularProgress)
		{
			PrimaryMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        
			// Apply the dynamic material to the UImage
			PrimaryCircularProgress->SetBrushFromMaterial(PrimaryMaterialInstance);
		
		}

		if (SecondaryCircularProgress)
		{
			SecondaryMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);

			// Apply the dynamic material to the UImage
			SecondaryCircularProgress->SetBrushFromMaterial(SecondaryMaterialInstance);
		
		}
	}
	
}

void UHUDWidget::UpdateCircularProgressBar(UMaterialInstanceDynamic* MaterialInstance, float ProgressValue)
{
	if (MaterialInstance)
	{
		// Set the scalar parameter (e.g., ProgressValue) to control the progress in the material
		MaterialInstance->SetScalarParameterValue("Progress", ProgressValue);
	}
}

