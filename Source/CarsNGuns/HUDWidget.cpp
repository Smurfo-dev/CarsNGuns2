// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "BaseWeapon.h"
#include "BasePhysicsVehiclePawn.h"
#include "Components/Image.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHUDWidget::SetWeaponIcons()
{
	if (PlayerReference->GetPrimaryWeapon() && PlayerReference->GetPrimaryWeapon()->GetWeaponIcon())
		PrimaryWeaponIcon->SetBrushFromTexture(PlayerReference->GetPrimaryWeapon()->GetWeaponIcon());

	if (PlayerReference->GetSecondaryWeapon() && PlayerReference->GetSecondaryWeapon()->GetWeaponIcon())
		SecondaryWeaponIcon->SetBrushFromTexture(PlayerReference->GetSecondaryWeapon()->GetWeaponIcon());
}
