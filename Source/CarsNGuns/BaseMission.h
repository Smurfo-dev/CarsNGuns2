// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseMission.generated.h"

UENUM(BlueprintType)
enum class EMissionState : uint8
{
	Inactive,
	Active,
	Completed,
	Failed
};

UCLASS()
class CARSNGUNS_API ABaseMission : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseMission();

	virtual void StartEvent();
	virtual void EndEvent(bool bSuccess);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Marker")
	FLinearColor MarkerColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, Category="Marker")
	class UWidgetComponent* MissionMarkerWidgetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Marker")
	class USphereComponent* MissionTriggerZone;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Marker")
	UStaticMeshComponent* MissionTriggerMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	EMissionState MissionState;

	UPROPERTY()
	class UMissionMarkerWidget* MissionMarkerWidget;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnPlayerEnterMissionArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerExitMissionArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
