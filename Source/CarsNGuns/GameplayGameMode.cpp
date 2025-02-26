// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayGameMode.h"

#include "BasePhysicsVehiclePawn.h"
#include "DefaultGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "WeaponSelectionMenu.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerStart.h"

class UEnhancedInputLocalPlayerSubsystem;

AGameplayGameMode::AGameplayGameMode()
{
	StartTime = 0.0f;
	PausedTime = 0.0f;
	bIsTimerActive = true;
}

void AGameplayGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetupPlayer();

	StartTime = GetWorld()->GetTimeSeconds();

	GetWorldTimerManager().SetTimerForNextTick(this, &AGameplayGameMode::ShowWeaponSelectionMenu);
}

void AGameplayGameMode::SetupPlayer()
{
	if(UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		SelectedPlayerPawnClass = DefaultPawnClass;
		//Prioriterar class vald i gamemode blueprint
		if(!SelectedPlayerPawnClass && GameInstance->GetSelectedPlayerPawnClass()) //Prevents crashing vid start av nivå utan default pawn och klass satt i game instance
		{
			UE_LOG(LogTemp, Warning, TEXT("Starting with selected player pawn class"));
			SelectedPlayerPawnClass = GameInstance->GetSelectedPlayerPawnClass(); //If no default is chosen, select from game instance
			
			if(AActor* PlayerStart = UGameplayStatics::GetActorOfClass(this, APlayerStart::StaticClass()))
			{
				APawn* PawnToSpawn = GetWorld()->SpawnActor<APawn>(SelectedPlayerPawnClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
				if(PawnToSpawn && PlayerController) PlayerController->Possess(PawnToSpawn);
			}
		}
		
		if(SelectedPlayerPawnClass)
		{
			if(PlayerController)
			{
				// Ensure Enhanced Input is setup
				UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
				if (Subsystem)
				{
					// Add the Input Mapping Context for the pawn
					Subsystem->AddMappingContext(VehicleMappingContext, 1);  // Adjust the priority as needed
					UE_LOG(LogTemp, Warning, TEXT("Input Mapping Context added in GameMode."));
				}
				PlayerController->SetInputMode(FInputModeGameOnly());
				PlayerController->bShowMouseCursor = false;
			}
		}
		GameInstance->PopulateEnemies();
	}
}

void AGameplayGameMode::ShowWeaponSelectionMenu()
{
	//Spawn Weapon Selection Menu
	if(WeaponSelectionMenuClass && AvailableWeapons.Num() > 1)
	{
		int32 Index1 = FMath::RandRange(0, AvailableWeapons.Num() -1 );
		int32 Index2;
		do
		{
			Index2 = FMath::RandRange(0, AvailableWeapons.Num() -1 );
		} while (Index1 == Index2);

		
		UWeaponSelectionMenu* WeaponSelectionMenu = CreateWidget<UWeaponSelectionMenu>(GetWorld(), WeaponSelectionMenuClass);
		if (WeaponSelectionMenu)
		{
			WeaponSelectionMenu->AddToViewport();
			WeaponSelectionMenu->InitializeMenu(AvailableWeapons[Index1], AvailableWeapons[Index2]);
			
			if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
			{
				UGameplayStatics::SetGamePaused(GetWorld(), true);
				const FInputModeUIOnly InputMode;
				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = true;
				
				SetTimerActive(false);
			}

			WeaponSelectionMenu->OnMenuClosed.AddDynamic(this, &AGameplayGameMode::OnWeaponSelectionMenuClosed);
		}
	}
}

void AGameplayGameMode::OnWeaponSelectionMenuClosed()
{
	// Resume the game
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false;
		SetTimerActive(true);
	}
}

float AGameplayGameMode::GetLevelTime() const
{
	if(bIsTimerActive) return GetWorld()->GetTimeSeconds() - StartTime;

	return PausedTime;
}

void AGameplayGameMode::SetTimerActive(bool bActive)
{
	if(bActive && !bIsTimerActive) StartTime += GetWorld()->GetTimeSeconds() - PausedTime;
	else if(!bActive && bIsTimerActive) PausedTime = GetWorld()->GetTimeSeconds() - StartTime;

	bIsTimerActive = bActive;
}
