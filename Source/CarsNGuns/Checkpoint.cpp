// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "Components/SphereComponent.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CheckpointTriggerZone = CreateDefaultSubobject<USphereComponent>(TEXT("CheckpointTriggerZone"));
	CheckpointTriggerZone->InitSphereRadius(1000.0f);
	CheckpointTriggerZone->SetGenerateOverlapEvents(true);
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
	
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpoint::OnPlayerExitMissionArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void ACheckpoint::OnPlayerEnterMissionArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}



