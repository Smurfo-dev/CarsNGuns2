// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "BasePhysicsVehiclePawn.h"
#include "HUDWidget.h"
#include "CrosshairWidget.h"
#include "MissionInfoWidget.h"
#include "DebugMenuWidget.h"
#include "Blueprint/UserWidget.h"

AMyPlayerController::AMyPlayerController()
{
	// Set this player controller to use your custom HUD widget if needed
	/*
	static ConstructorHelpers::FClassFinder<UUserWidget> HUDWidgetClassFinder(TEXT("/Game/Path/To/YourWidget"));
	if (HUDWidgetClassFinder.Succeeded())
	{
		HUDWidgetClass = HUDWidgetClassFinder.Class;
	}
	*/ //JUST SET IT IN BLUEPRINT SMH
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (CrosshairWidgetClass)
	{
		CurrentCrosshairWidget = CreateWidget<UCrosshairWidget>(this, CrosshairWidgetClass);
		if (CurrentCrosshairWidget)
		{
			CurrentCrosshairWidget->AddToViewport();
			CurrentCrosshairWidget->PlayerReference = Cast<ABasePhysicsVehiclePawn>(GetPawn());
		}
	}

	if (HUDWidgetClass)
	{
		CurrentHUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
		if (CurrentHUDWidget)
		{
			UDefaultGameInstance* DefaultGameInstance = GetWorld()->GetGameInstance<UDefaultGameInstance>();
			CurrentHUDWidget->AddToViewport();
			CurrentHUDWidget->PlayerReference = Cast<ABasePhysicsVehiclePawn>(GetPawn());
			CurrentHUDWidget->DefaultGameInstance = DefaultGameInstance;
		}
	}
}

void AMyPlayerController::ToggleDebugMenu()
{
	if (!CurrentDebugMenuWidget && DebugMenuWidgetClass)
	{
		CurrentDebugMenuWidget = CreateWidget<UDebugMenuWidget>(this, DebugMenuWidgetClass);
	}

	if (CurrentDebugMenuWidget)
	{
		if (CurrentDebugMenuWidget->IsInViewport())
		{
			UE_LOG(LogTemp, Warning, TEXT("Removing Debug Menu From Viewport"))
			CurrentDebugMenuWidget->RemoveFromParent();
			SetShowMouseCursor(false);
			FInputModeGameOnly InputMode;
			SetInputMode(InputMode);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Adding Debug Menu To Viewport"))
			CurrentDebugMenuWidget->AddToViewport();
			CurrentDebugMenuWidget->PlayerReference = Cast<ABasePhysicsVehiclePawn>(GetPawn());
			CurrentDebugMenuWidget->PopulateWeaponDropDowns();
			SetShowMouseCursor(true);
			FInputModeGameAndUI InputMode;
			SetInputMode(InputMode);
		}
	}
}

void AMyPlayerController::ToggleMissionInfoMenu()
{
	if (!CurrentMissionInfoWidget && MissionInfoMenuWidgetClass)
	{
		CurrentMissionInfoWidget = CreateWidget<UMissionInfoWidget>(this, MissionInfoMenuWidgetClass);
	}

	if (CurrentMissionInfoWidget)
	{
		if (CurrentMissionInfoWidget->IsInViewport())
		{
			UE_LOG(LogTemp, Warning, TEXT("Removing Mission Info Menu From Viewport"))
			CurrentMissionInfoWidget->RemoveFromParent();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Adding Mission Info Menu To Viewport"))
			CurrentMissionInfoWidget->AddToViewport();
		}
	}
}

void AMyPlayerController::HandlePlayerDeath()
{
	if (CurrentCrosshairWidget) CurrentCrosshairWidget->RemoveFromParent();
	if (CurrentHUDWidget) CurrentHUDWidget->RemoveFromParent();
	if (CurrentDebugMenuWidget) CurrentDebugMenuWidget->RemoveFromParent();
	
}

void AMyPlayerController::UpdateWeaponIcons()
{
	if (CurrentHUDWidget) CurrentHUDWidget->SetWeaponIcons();
}
