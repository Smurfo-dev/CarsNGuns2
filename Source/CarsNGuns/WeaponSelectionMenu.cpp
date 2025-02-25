// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSelectionMenu.h"

#include "PlayerVehicleBase.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UWeaponSelectionMenu::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerPawn = Cast<APlayerVehicleBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (WeaponButton1)
	{
		WeaponButton1->OnClicked.AddDynamic(this, &UWeaponSelectionMenu::OnWeapon1Selected);
	}
	if (WeaponButton2)
	{
		WeaponButton2->OnClicked.AddDynamic(this, &UWeaponSelectionMenu::OnWeapon2Selected);
	}
}

void UWeaponSelectionMenu::InitializeMenu(TSubclassOf<ABaseWeapon> Weapon1, TSubclassOf<ABaseWeapon> Weapon2)
{
	SelectedWeapon1 = Weapon1;
	SelectedWeapon2 = Weapon2;
	
	if (WeaponText1)
	{
		WeaponText1->SetText(FText::FromString(Weapon1->GetName()));
	}
	if (WeaponText2)
	{
		WeaponText2->SetText(FText::FromString(Weapon2->GetName()));
	}
}

void UWeaponSelectionMenu::OnWeapon1Selected()
{
	if(PlayerPawn)
	{
		PlayerPawn->AttachWeaponToVehicle(SelectedWeapon1);
	}
	CloseMenu();
}

void UWeaponSelectionMenu::OnWeapon2Selected()
{
	if(PlayerPawn)
	{
		PlayerPawn->AttachWeaponToVehicle(SelectedWeapon2);
	}
	CloseMenu();
}

void UWeaponSelectionMenu::CloseMenu()
{
	OnMenuClosed.Broadcast();
	RemoveFromParent();
}

