// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "Components/SphereComponent.h"
#include "BaseMission.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CheckpointTriggerZone = CreateDefaultSubobject<USphereComponent>(TEXT("CheckpointTriggerZone"));
	CheckpointTriggerZone->InitSphereRadius(200.0f);
	CheckpointTriggerZone->SetGenerateOverlapEvents(false);
	CheckpointTriggerZone->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointTriggerZone->SetCollisionObjectType(ECC_WorldDynamic);
	CheckpointTriggerZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	CheckpointTriggerZone->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	CheckpointTriggerZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CheckpointTriggerZone;

	CheckpointTriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointTriggerMesh"));
	CheckpointTriggerMesh->SetupAttachment(RootComponent);
	CheckpointTriggerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CheckpointTriggerMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	CheckpointTriggerMesh->SetVisibility(false);
}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	CheckpointTriggerZone->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnPlayerEnterCheckpoint);
	
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpoint::EnableCheckpoint()
{
	if (CheckpointTriggerMesh && CheckpointTriggerZone)
	{
		CheckpointTriggerMesh->SetVisibility(true);
		CheckpointTriggerZone->SetGenerateOverlapEvents(true);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Enabling Checkpoint!")));	
	}
}

void ACheckpoint::DisableCheckpoint()
{
	if (CheckpointTriggerMesh && CheckpointTriggerZone)
	{
		CheckpointTriggerMesh->SetVisibility(false);
		CheckpointTriggerZone->SetGenerateOverlapEvents(false);
	}
}

void ACheckpoint::OnPlayerEnterCheckpoint(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	MissionReference->ActivateCheckpoint();
}



