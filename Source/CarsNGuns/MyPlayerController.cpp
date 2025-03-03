// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "BasePhysicsVehiclePawn.h"
#include "HUDWidget.h"
#include "CrosshairWidget.h"
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
			CurrentHUDWidget->AddToViewport();
			CurrentHUDWidget->PlayerReference = Cast<ABasePhysicsVehiclePawn>(GetPawn());
		}
	}
}

void AMyPlayerController::HandlePlayerDeath()
{
	if (CurrentCrosshairWidget) CurrentCrosshairWidget->RemoveFromParent();
	if (CurrentHUDWidget) CurrentHUDWidget->RemoveFromParent();
}
