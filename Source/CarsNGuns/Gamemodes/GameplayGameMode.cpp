// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayGameMode.h"

#include "CarsNGuns/Vehicles/BasePhysicsVehiclePawn.h"
#include "CarsNGuns/Systems/DefaultGameInstance.h"
#include "CarsNGuns/Systems/DefaultGameState.h"
#include "Kismet/GameplayStatics.h"
#include "CarsNGuns/Components/MissionUpgradeComponent.h"
#include "CarsNGuns/Weapons/BaseWeapon.h"
#include "Blueprint/UserWidget.h"
#include "CarsNGuns/Widgets/WeaponSelectionMenu.h"
#include "EnhancedInputSubsystems.h"
#include "CarsNGuns/Player/MyPlayerController.h"
#include "GameFramework/PlayerStart.h"

class UEnhancedInputLocalPlayerSubsystem;

AGameplayGameMode::AGameplayGameMode()
{
	GameStateClass = ADefaultGameState::StaticClass();
}

void AGameplayGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimerForNextTick(this, &AGameplayGameMode::SetupPlayer);
	
	UDefaultGameInstance* DefaultGameInstance = GetWorld()->GetGameInstance<UDefaultGameInstance>();
	DefaultGameInstance->StartTimer();

	//Använd den här som exempel på hur du kan läsa från the cursed map!!
	const TMultiMap<EWeaponType, TSharedPtr<FUpgrade>>* WeaponEnhancementUpgrades = DefaultGameInstance->GetAvailableUpgrades().Find(EUpgradeType::WeaponEnhancement);

	if (WeaponEnhancementUpgrades && WeaponEnhancementUpgrades->Num() > 0)
	{
		// If upgrades are found, log how many there are
		UE_LOG(LogTemp, Warning, TEXT("Available Weapon Enhancement upgrades: %d"), WeaponEnhancementUpgrades->Num());
	}
	else
	{
		// If no upgrades found, log a message
		UE_LOG(LogTemp, Warning, TEXT("No Weapon Enhancement upgrades available."));
	}

	
	if (WeaponSelectionMenuClass) GetWorldTimerManager().SetTimerForNextTick(this, &AGameplayGameMode::ShowWeaponSelectionMenu);
}

void AGameplayGameMode::SetupPlayer()
{
	
	if(UDefaultGameInstance* DefaultGameInstance = GetWorld()->GetGameInstance<UDefaultGameInstance>())
	{
		AMyPlayerController* PlayerController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		SelectedPlayerPawnClass = DefaultPawnClass;
		//Prioriterar class vald i gamemode blueprint
		if(!SelectedPlayerPawnClass && DefaultGameInstance->GetSelectedPlayerPawnClass()) //Prevents crashing vid start av nivå utan default pawn och klass satt i game instance
		{
			UE_LOG(LogTemp, Warning, TEXT("Starting with selected player pawn class"));
			SelectedPlayerPawnClass = DefaultGameInstance->GetSelectedPlayerPawnClass(); //If no default is chosen, select from game instance
			
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
			}
		}
		if (PlayerController)
		{
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->bShowMouseCursor = false;  // or true depending on your needs
		}
		
		ADefaultGameState* DefaultGameState = GetWorld()->GetGameState<ADefaultGameState>();
		ABasePhysicsVehiclePawn* PlayerVehicleReference = Cast<ABasePhysicsVehiclePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		DefaultGameState->SetPlayerController(PlayerController);
		DefaultGameState->PopulateEnemies();
		DefaultGameState->InitializeMissionManager(PlayerVehicleReference);
	}
}

void AGameplayGameMode::ShowWeaponSelectionMenu()
{

	UDefaultGameInstance* DefaultGameInstance = GetWorld()->GetGameInstance<UDefaultGameInstance>();
	if (DefaultGameInstance)
	{
		//Spawn Weapon Selection Menu
		if(WeaponSelectionMenuClass && DefaultGameInstance->AvailableWeapons.Num() > 1)
		{
			int32 Index1 = FMath::RandRange(0, DefaultGameInstance->AvailableWeapons.Num() -1 );
			int32 Index2;
			do
			{
				Index2 = FMath::RandRange(0, DefaultGameInstance->AvailableWeapons.Num() -1 );
			} while (Index1 == Index2);

		
			UWeaponSelectionMenu* WeaponSelectionMenu = CreateWidget<UWeaponSelectionMenu>(GetWorld(), WeaponSelectionMenuClass);
			if (WeaponSelectionMenu)
			{
				WeaponSelectionMenu->AddToViewport();
				WeaponSelectionMenu->InitializeMenu(DefaultGameInstance->AvailableWeapons[Index1], DefaultGameInstance->AvailableWeapons[Index2]);
			
				if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
				{
					UGameplayStatics::SetGamePaused(GetWorld(), true);
					const FInputModeUIOnly InputMode;
					PlayerController->SetInputMode(InputMode);
					PlayerController->bShowMouseCursor = true;
					DefaultGameInstance->PauseTimer();
				}

				WeaponSelectionMenu->OnMenuClosed.AddDynamic(this, &AGameplayGameMode::OnWeaponSelectionMenuClosed);
			}
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
		UDefaultGameInstance* DefaultGameInstance = GetWorld()->GetGameInstance<UDefaultGameInstance>();
		DefaultGameInstance->ResumeTimer();
	}
}
