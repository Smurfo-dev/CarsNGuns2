// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMission.h"

#include "BasePhysicsVehiclePawn.h"
#include "Components/WidgetComponent.h"
#include "MissionMarkerWidget.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseMission::ABaseMission()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MissionMarkerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("MissionMarkerWidget"));
	MissionMarkerWidgetComponent->SetupAttachment(RootComponent);

	MissionMarkerWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

}

// Called when the game starts or when spawned
void ABaseMission::BeginPlay()
{
	Super::BeginPlay();

	if (MissionMarkerWidgetComponent)
	{
		if (UUserWidget* WidgetInstance = MissionMarkerWidgetComponent->GetWidget())
		{
			MissionMarkerWidget = Cast<UMissionMarkerWidget>(WidgetInstance);
			if (MissionMarkerWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("Mission Marker Mission Reference Set"))
				MissionMarkerWidget->MissionReference = this;
				if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
				{
					if (ABasePhysicsVehiclePawn* PlayerVehicle = Cast<ABasePhysicsVehiclePawn>(PlayerController->GetPawn()))
					{
						UE_LOG(LogTemp, Warning, TEXT("Mission Marker Player Reference Set"))
						MissionMarkerWidget->PlayerReference = PlayerVehicle;

						MissionMarkerWidget->MarkerImage->SetBrushTintColor(MarkerColor);
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

void ABaseMission::StartEvent()
{
	
}

void ABaseMission::EndEvent(bool bSuccess)
{

	
}

