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

	if (PlayerReference->GetPrimaryWeapon())
	{
		if(EquippedPrimaryID == "Machine Gun")
		{
			
		}
		else if (EquippedPrimaryID == "Rocket Launcher")
		{
			
		}
		else if (EquippedPrimaryID == "Laser Rifle")
		{
			PrimaryProgress->SetPercent(CalculateProgress(EquippedPrimaryID, 0));
		}
		else if (EquippedPrimaryID == "Grenade Launcher")
		{
			
		}
		else
		{
			
		}
	}
	if (PlayerReference->GetSecondaryWeapon())
	{
		if(EquippedSecondaryID == "Machine Gun")
		{
			
		}
		else if (EquippedSecondaryID == "Rocket Launcher")
		{
			
		}
		else if (EquippedSecondaryID == "Laser Rifle")
		{
			SecondaryProgress->SetPercent(CalculateProgress(EquippedSecondaryID, 1));
		}
		else if (EquippedSecondaryID == "Grenade Launcher")
		{
			
		}
		else
		{
			
		}
	}
}

float UHUDWidget::CalculateProgress(FString WeaponID, int32 WeaponIndex)
{
	if (WeaponID == "Machine Gun")
	{
		
	}
	else if (WeaponID ==  "Rocket Launcher")
	{
		
	}
	else if (WeaponID == "Laser Rifle")
	{
		if (WeaponIndex == 0) return PlayerReference->GetPrimaryWeapon()->GetResourceBarValue();
		return PlayerReference->GetSecondaryWeapon()->GetResourceBarValue();
	}
	else if (WeaponID == "Grenade Launcher")
	{
		
	}
	else
	{
		
	}
	return 0;
	
}

void UHUDWidget::SetBarVisibilityPrimary(FString EquipID)
{
	UE_LOG(LogTemp, Warning, TEXT("Set Bar Visibility: %s"), *EquipID);
	if (EquipID == "Machine Gun")
	{
		PrimaryCircularProgress->SetVisibility(ESlateVisibility::Hidden);
		PrimaryProgress->SetVisibility(ESlateVisibility::Visible);
	}
	else if (EquipID == "Rocket Launcher")
	{
		PrimaryProgress->SetVisibility(ESlateVisibility::Hidden);
		PrimaryCircularProgress->SetVisibility(ESlateVisibility::Visible);
	}
	else if (EquipID == "Laser Rifle")
	{
		PrimaryCircularProgress->SetVisibility(ESlateVisibility::Hidden);
		PrimaryProgress->SetVisibility(ESlateVisibility::Visible);
	}
	else if (EquipID == "Grenade Launcher")
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
	if (EquipID == "Machine Gun")
	{
		SecondaryCircularProgress->SetVisibility(ESlateVisibility::Hidden);
		SecondaryProgress->SetVisibility(ESlateVisibility::Visible);
	}
	else if (EquipID == "Rocket Launcher")
	{
		SecondaryProgress->SetVisibility(ESlateVisibility::Hidden);
		SecondaryCircularProgress->SetVisibility(ESlateVisibility::Visible);
	}
	else if (EquipID == "Laser Rifle")
	{
		SecondaryCircularProgress->SetVisibility(ESlateVisibility::Hidden);
		SecondaryProgress->SetVisibility(ESlateVisibility::Visible);
	}
	else if (EquipID == "Grenade Launcher")
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
}
