// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

UCLASS()
class CARSNGUNS_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Marker")
	class USphereComponent* CheckpointTriggerZone;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Marker")
	UStaticMeshComponent* CheckpointTriggerMesh;

	void SetMissionReference(class ABaseMission* Mission)
	{
		MissionReference = Mission;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void EnableCheckpoint();

	void DisableCheckpoint();

private:
	UFUNCTION()
	void OnPlayerEnterCheckpoint(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	ABaseMission* MissionReference;
};
