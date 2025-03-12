// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMission.h"

#include "BasePhysicsVehiclePawn.h"
#include "Components/WidgetComponent.h"
#include "MyPlayerController.h"
#include "MissionMarkerWidget.h"
#include "PlayerVehicleBase.h"
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
}

// Called when the game starts or when spawned
void ABaseMission::BeginPlay()
{
	Super::BeginPlay();

	MissionTriggerZone->OnComponentBeginOverlap.AddDynamic(this, &ABaseMission::OnPlayerEnterMissionArea);
	MissionTriggerZone->OnComponentEndOverlap.AddDynamic(this, &ABaseMission::OnPlayerExitMissionArea);

	if (MissionMarkerWidgetComponent)
	{
		if (UUserWidget* WidgetInstance = MissionMarkerWidgetComponent->GetWidget())
		{
			MissionMarkerWidget = Cast<UMissionMarkerWidget>(WidgetInstance);
			if (MissionMarkerWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("Mission Marker Mission Reference Set"))
				MissionMarkerWidget->MissionReference = this;
				PlayerController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
				if (PlayerController)
				{
					if (ABasePhysicsVehiclePawn* PlayerVehicle = Cast<ABasePhysicsVehiclePawn>(PlayerController->GetPawn()))
					{
						UE_LOG(LogTemp, Warning, TEXT("Mission Marker Player Reference Set"))
						MissionMarkerWidget->PlayerReference = PlayerVehicle;

						MissionMarkerWidget->MarkerImage->SetBrushTintColor(MarkerColor);
						MissionTriggerMesh->SetVisibility(true);
					}
				}
			}
		}
	}
}

// Called every frame
void ABaseMission::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseMission::OnPlayerEnterMissionArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<APlayerVehicleBase>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Player Entered Mission Area"));
		MissionTriggerMesh->SetVisibility(false);
		MissionMarkerWidget->SetVisibility(ESlateVisibility::Hidden);

		Cast<APlayerVehicleBase>(OtherActor)->SetActiveMissionZone(this);
		if (PlayerController) PlayerController->ToggleMissionInfoMenu();
	}
}

void ABaseMission::OnPlayerExitMissionArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<APlayerVehicleBase>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player Left Mission Area"));
		MissionTriggerMesh->SetVisibility(true);
		MissionMarkerWidget->SetVisibility(ESlateVisibility::Visible);

		Cast<APlayerVehicleBase>(OtherActor)->SetActiveMissionZone(nullptr);
		if (PlayerController) PlayerController->ToggleMissionInfoMenu();
	}
}

void ABaseMission::OnMissionStateChanged(const EMissionState NewState)
{
	switch (NewState)
	{
		case EMissionState::Active:
			SetActorHiddenInGame(false);
			break;
		case EMissionState::Inactive:
			SetActorHiddenInGame(true);
			break;
		case EMissionState::Completed:
			break;
		case EMissionState::Failed:
			break;
	}
}


void ABaseMission::StartEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Starting Event!")));
}

void ABaseMission::EndEvent(bool bSuccess)
{

	
}

