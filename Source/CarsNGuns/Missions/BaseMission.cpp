// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMission.h"

#include "CarsNGuns/Vehicles/BasePhysicsVehiclePawn.h"
#include "Components/WidgetComponent.h"
#include "CarsNGuns/Player/MyPlayerController.h"
#include "CarsNGuns/Systems/DefaultGameInstance.h"
#include "CarsNGuns/Widgets/MissionMarkerWidget.h"
#include "CarsNGuns/Widgets/MissionInfoWidget.h"
#include "CarsNGuns/Widgets/UpgradeSelectionWidget.h"
#include "CarsNGuns/Systems/DefaultGameState.h"
#include "CarsNGuns/Vehicles/PlayerVehicleBase.h"
#include "Components/Image.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseMission::ABaseMission()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MissionTriggerZone = CreateDefaultSubobject<USphereComponent>(TEXT("MissionTriggerZone"));
	MissionTriggerZone->InitSphereRadius(1000.0f);
	MissionTriggerZone->SetGenerateOverlapEvents(true);
	MissionTriggerZone->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MissionTriggerZone->SetCollisionObjectType(ECC_WorldDynamic);
	MissionTriggerZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	MissionTriggerZone->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	MissionTriggerZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = MissionTriggerZone;

	MissionTriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MissionTriggerMesh"));
	MissionTriggerMesh->SetupAttachment(RootComponent);
	MissionTriggerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MissionTriggerMesh->SetCollisionResponseToAllChannels(ECR_Ignore); 

	MissionMarkerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("MissionMarkerWidget"));
	MissionMarkerWidgetComponent->SetupAttachment(RootComponent);

	MissionMarkerWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	MissionUpgradeComponent = CreateDefaultSubobject<UMissionUpgradeComponent>(TEXT("MissionUpgradeComponent"));
}

// Called when the game starts or when spawned
void ABaseMission::BeginPlay()
{
	Super::BeginPlay();

	MissionTriggerZone->OnComponentBeginOverlap.AddDynamic(this, &ABaseMission::OnPlayerEnterMissionArea);
	MissionTriggerZone->OnComponentEndOverlap.AddDynamic(this, &ABaseMission::OnPlayerExitMissionArea);

	DefaultGameState = GetWorld()->GetGameState<ADefaultGameState>();
	DefaultGameInstance = GetWorld()->GetGameInstance<UDefaultGameInstance>();

	if (MissionMarkerWidgetComponent)
	{
		if (UUserWidget* WidgetInstance = MissionMarkerWidgetComponent->GetWidget())
		{
			MissionMarkerWidget = Cast<UMissionMarkerWidget>(WidgetInstance);
			if (MissionMarkerWidget)
			{
				MissionMarkerWidget->MissionReference = this;
				PlayerController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
				if (PlayerController)
				{
					if (ABasePhysicsVehiclePawn* PlayerVehicle = Cast<ABasePhysicsVehiclePawn>(PlayerController->GetPawn()))
					{
						MissionMarkerWidget->PlayerReference = PlayerVehicle;

						MissionMarkerWidget->MarkerImage->SetBrushTintColor(MarkerColor);
						MissionTriggerMesh->SetVisibility(true);
					}
				}
			}
		}
	}
}

void ABaseMission::ActivateCheckpoint()
{
	//Override when needed
}

// Called every frame
void ABaseMission::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bMissionStarted) GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Yellow, FString::Printf(TEXT("Mission Time: %.2f"), GetMissionTime()));
	
}

void ABaseMission::OnPlayerEnterMissionArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<APlayerVehicleBase>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Player Entered Mission Area"));
		if (MissionState == EMissionState::Active)
		{
			MissionTriggerMesh->SetVisibility(false);
			MissionMarkerWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		Cast<APlayerVehicleBase>(OtherActor)->SetActiveMissionZone(this);
		
		ShowMissionInfo();
			
	}
}

void ABaseMission::OnPlayerExitMissionArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<APlayerVehicleBase>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player Left Mission Area"));
		if (MissionState == EMissionState::Active)
		{
			MissionTriggerMesh->SetVisibility(true);
			MissionMarkerWidget->SetVisibility(ESlateVisibility::Visible);
		}
		

		Cast<APlayerVehicleBase>(OtherActor)->SetActiveMissionZone(nullptr);

		HideMissionInfo();
		
	}
}

void ABaseMission::OnMissionStateChanged(const EMissionState NewState)
{
	switch (NewState)
	{
		case EMissionState::Active:
			EnableMission();
			break;
		case EMissionState::Inactive:
			DisableMission();
			break;
		case EMissionState::Completed:
			DisableMission();
			break;
		case EMissionState::Failed:
			MissionMarkerWidget->MarkerImage->SetBrushTintColor(EStyleColor::Black);
			break;
	}
}


void ABaseMission::StartEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Starting Event!")));
	HideMissionInfo();
	MissionStartTime = DefaultGameInstance->GetElapsedTime();
	bMissionStarted = true;
}

void ABaseMission::EndEvent(bool bSuccess)
{
	if (bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mission Success, trying to fill upgrades array in mission upgrade component!!"))
		SetMissionState(EMissionState::Completed);
		MissionUpgradeComponent->GetUpgrades();
		for (auto Upgrade : MissionUpgradeComponent->Upgrades)
		{
			UE_LOG(LogTemp, Warning, TEXT("Upgrade Array: %s"), *Upgrade.DisplayName)
		}
		ShowMissionRewards(MissionUpgradeComponent->Upgrades);
	}
	else SetMissionState(EMissionState::Failed);

	bMissionStarted = false;
}

void ABaseMission::ShowMissionInfo()
{
	if (!CurrentMissionInfoWidget && MissionInfoMenuWidgetClass)
	{
		CurrentMissionInfoWidget = CreateWidget<UMissionInfoWidget>(GetWorld(), MissionInfoMenuWidgetClass);
	}
	if (CurrentMissionInfoWidget && !CurrentMissionInfoWidget->IsInViewport())
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Mission Info Menu To Viewport"))
		CurrentMissionInfoWidget->AddToViewport();
		CurrentMissionInfoWidget->InitializeValues(MissionUpgradeComponent->MissionInfo);
	}
}

void ABaseMission::HideMissionInfo()
{
	if (CurrentMissionInfoWidget)
	{
		if (CurrentMissionInfoWidget->IsInViewport())
		{
			UE_LOG(LogTemp, Warning, TEXT("Removing Mission Info Menu From Viewport"))
			CurrentMissionInfoWidget->RemoveFromParent();
		}
	}
}

void ABaseMission::ShowMissionRewards(const TArray<FUpgrade>& Upgrades)
{
	if (!UpgradeSelectionWidget && UpgradeSelectionWidgetClass)
	{
		UpgradeSelectionWidget = CreateWidget<UUpgradeSelectionWidget>(GetWorld(), UpgradeSelectionWidgetClass);
	}
	if (UpgradeSelectionWidget && !UpgradeSelectionWidget->IsInViewport())
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Mission Reward Menu To Viewport"))
		UpgradeSelectionWidget->AddToViewport();
		UpgradeSelectionWidget->InitializeValues(Upgrades, PlayerController, this);
	}
}

void ABaseMission::DisableMission()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Disabling Mission: "))+ MissionUpgradeComponent->MissionInfo.MissionName);
	MissionTriggerZone->SetVisibility(false);
	MissionTriggerZone->SetGenerateOverlapEvents(false);
	MissionTriggerMesh->SetVisibility(false);
	MissionMarkerWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ABaseMission::EnableMission()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Enabling Mission: "))+ MissionUpgradeComponent->MissionInfo.MissionName);
	MissionTriggerZone->SetVisibility(true);
	MissionTriggerZone->SetGenerateOverlapEvents(true);
	MissionTriggerMesh->SetVisibility(true);
	MissionMarkerWidget->SetVisibility(ESlateVisibility::Visible);
}

void ABaseMission::ApplyRewards(int32 UpgradeIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Applying Rewards!!!")));
	if (MissionUpgradeComponent->Upgrades.IsValidIndex(UpgradeIndex))
	{
		const FUpgrade& Upgrade = MissionUpgradeComponent->Upgrades[UpgradeIndex];
		switch (Upgrade.UpgradeType)
		{
		case EUpgradeType::WeaponEnhancement:
			//PlayerReference->ApplyWeaponEnhancement(Upgrade);
				break;
		case EUpgradeType::WeaponAugment:
			//PlayerReference->ApplyWeaponAugment(Upgrade);
				break;
		case EUpgradeType::VehicleModification:
			//PlayerReference->ApplyVehicleModification(Upgrade);
				break;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Upgrade Choice"))
	}
}

void ABaseMission::PrintMissionInfo()
{
	FString MissionText = FString::Printf(TEXT("%s: %s"), *MissionUpgradeComponent->MissionInfo.MissionName, *GetMissionStateAsString(MissionState));
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, MissionText);
}

float ABaseMission::GetMissionTime()
{
	return GetWorld()->GetTimeSeconds() - MissionStartTime;
}


