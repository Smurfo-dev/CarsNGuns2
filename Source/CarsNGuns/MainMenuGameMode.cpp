// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"

#include "DefaultGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/CinematicCamera/Public/CineCameraActor.h"

AMainMenuGameMode::AMainMenuGameMode() : MainMenuWidget(nullptr), ChooseVehicleWidget(nullptr), CinematicCamera(nullptr) //initialize with nullptrs 
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMainMenuGameMode::BeginPlay()
{
	//Get the camera and set the camera as viewtarget
	CinematicCamera = Cast<ACineCameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ACineCameraActor::StaticClass()));

	if(CinematicCamera)
	{
		if(APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			PlayerController->SetViewTarget(CinematicCamera);
		}
		BaseCameraLocation = CinematicCamera->GetActorLocation();
		BaseCameraRotation = CinematicCamera->GetActorRotation();
	}
	
	if(MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
		if(MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();

			UButton* StartButton = Cast<UButton>(MainMenuWidget->GetWidgetFromName(TEXT("StartGameButton")));
			UButton* ExitButton = Cast<UButton>(MainMenuWidget->GetWidgetFromName(TEXT("ExitGameButton")));

			if(StartButton)
			{
				StartButton->OnClicked.AddDynamic(this, &AMainMenuGameMode::OnStartGamePressed);
			}
			if(ExitButton)
			{
				ExitButton->OnClicked.AddDynamic(this, &AMainMenuGameMode::OnExitGamePressed);
			}

			//Enables the mouse cursor
			if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
			{
				PlayerController->bShowMouseCursor = true;
				PlayerController->SetInputMode(FInputModeUIOnly());
			}
		}
	}

	if(ChooseVehicleWidgetClass)
	{
		ChooseVehicleWidget = CreateWidget<UUserWidget>(GetWorld(), ChooseVehicleWidgetClass);
		if(ChooseVehicleWidget)
		{
			UButton* MoveLeftButton = Cast<UButton>(ChooseVehicleWidget->GetWidgetFromName(TEXT("MoveLeftButton")));
			UButton* MoveRightButton = Cast<UButton>(ChooseVehicleWidget->GetWidgetFromName(TEXT("MoveRightButton")));
			UButton* MainMenuButton = Cast<UButton>(ChooseVehicleWidget->GetWidgetFromName(TEXT("MainMenuButton")));
			UButton* PlayGameButton = Cast<UButton>(ChooseVehicleWidget->GetWidgetFromName(TEXT("PlayGameButton")));
			AccelerationStatBar = Cast<UProgressBar>(ChooseVehicleWidget->GetWidgetFromName(TEXT("AccelerationStatBar")));
			TopSpeedStatBar = Cast<UProgressBar>(ChooseVehicleWidget->GetWidgetFromName(TEXT("TopSpeedStatBar")));
			HandlingStatBar = Cast<UProgressBar>(ChooseVehicleWidget->GetWidgetFromName(TEXT("HandlingStatBar")));
			ToughnessStatBar = Cast<UProgressBar>(ChooseVehicleWidget->GetWidgetFromName(TEXT("ToughnessStatBar")));
			VehicleNameTextBlock = Cast<UTextBlock>(ChooseVehicleWidget->GetWidgetFromName(TEXT("VehicleName")));
			WheelTypeNameTextBlock = Cast<UTextBlock>(ChooseVehicleWidget->GetWidgetFromName(TEXT("WheelTypeName")));

			if(MoveLeftButton)
			{
				MoveLeftButton->OnClicked.AddDynamic(this, &AMainMenuGameMode::OnMoveLeftPressed);
			}
			if(MoveRightButton)
			{
				MoveRightButton->OnClicked.AddDynamic(this, &AMainMenuGameMode::OnMoveRightPressed);
			}
			if(MainMenuButton)
			{
				MainMenuButton->OnClicked.AddDynamic(this, &AMainMenuGameMode::OnMainMenuPressed);
			}
			if(PlayGameButton)
			{
				PlayGameButton->OnClicked.AddDynamic(this, &AMainMenuGameMode::OnPlayPressed);
			}

			UpdateVehicleInfo();
		}
	}
}

void AMainMenuGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("Camera is moving: %d"), bCameraIsMoving);
	if(bCameraIsMoving)
	{
		CinematicCamera->SetActorLocation(FMath::VInterpTo(CinematicCamera->GetActorLocation(), TargetCameraLocation, DeltaTime, CameraMovementSpeed));
		CinematicCamera->SetActorRotation(FMath::RInterpTo(CinematicCamera->GetActorRotation(), TargetCameraRotation, DeltaTime, CameraMovementSpeed));

		if (FVector::Dist(CinematicCamera->GetActorLocation(), TargetCameraLocation) < 1.0f) // 1 unit tolerance
			{
				bCameraIsMoving = false; // Stop moving when close enough
			}
	}
}

void AMainMenuGameMode::OnStartGamePressed()
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		//PlayerController->bShowMouseCursor = false;
		//PlayerController->SetInputMode(FInputModeGameOnly());
		//Move camera
		//Swap widget to choose vehicles
		if(CinematicCamera) MoveCameraToVehicles();
		if(MainMenuWidget) MainMenuWidget->RemoveFromParent();
		if(ChooseVehicleWidget) ChooseVehicleWidget->AddToViewport();
	}
	
}

void AMainMenuGameMode::OnExitGamePressed()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}

void AMainMenuGameMode::OnPlayPressed()
{
	if(UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GameInstance->SetSelectedPlayerPawnClass(AvailableVehicleClasses[VehicleIndex]);
	}

	ChooseVehicleWidget->RemoveFromParent();
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->bShowMouseCursor = false;
	}
	
	UGameplayStatics::OpenLevel(this, FName(LevelToOpenOnStartGame));
}

void AMainMenuGameMode::OnMainMenuPressed()
{
	MoveCameraToTitle();
	ChooseVehicleWidget->RemoveFromParent();
	MainMenuWidget->AddToViewport();
}

void AMainMenuGameMode::OnMoveLeftPressed()
{
	if(VehicleIndex!=0)
	{
		MoveCameraLeft();
		VehicleIndex--;
		UpdateVehicleInfo();
	}
	UE_LOG(LogTemp, Warning, TEXT("Vehicle Index: %i"), VehicleIndex);
	UE_LOG(LogTemp, Warning, TEXT("Num of Available Vehicles: %i"), AvailableVehicleClasses.Num());
}

void AMainMenuGameMode::OnMoveRightPressed()
{
	if(VehicleIndex != AvailableVehicleClasses.Num()-1)
	{
		MoveCameraRight();
		VehicleIndex++;
		UpdateVehicleInfo();
	}
	UE_LOG(LogTemp, Warning, TEXT("Vehicle Index: %i"), VehicleIndex);
}

void AMainMenuGameMode::UpdateVehicleInfo()
{
	
	const ABasePhysicsVehiclePawn* CurrentVehicle = AvailableVehicleClasses[VehicleIndex]->GetDefaultObject<ABasePhysicsVehiclePawn>();
	AccelerationStatBar->SetPercent(CurrentVehicle->GetVehicleStats().Acceleration/10.0f);
	TopSpeedStatBar->SetPercent(CurrentVehicle->GetVehicleStats().TopSpeed/10.0f);
	HandlingStatBar->SetPercent(CurrentVehicle->GetVehicleStats().Handling/10.0f);
	ToughnessStatBar->SetPercent(CurrentVehicle->GetVehicleStats().Toughness/10.0f);
	VehicleNameTextBlock->SetText(FText::FromName(CurrentVehicle->GetVehicleStats().VehicleName));
	WheelTypeNameTextBlock->SetText(FText::FromName(CurrentVehicle->GetVehicleStats().WheelType));
	
}

void AMainMenuGameMode::MoveCameraToVehicles()
{
	TargetCameraLocation = LatestCameraLocation;
	TargetCameraRotation = LatestCameraRotation;
	bCameraIsMoving = true;
	
}

void AMainMenuGameMode::MoveCameraToTitle()
{
	TargetCameraLocation = BaseCameraLocation;
	TargetCameraRotation = BaseCameraRotation;
	bCameraIsMoving = true;
}

void AMainMenuGameMode::MoveCameraLeft()
{
	TargetCameraLocation.X+=500;
	LatestCameraLocation = TargetCameraLocation;
	bCameraIsMoving = true;
}

void AMainMenuGameMode::MoveCameraRight()
{
	TargetCameraLocation.X-=500;
	LatestCameraLocation = TargetCameraLocation;
	bCameraIsMoving = true;
}


