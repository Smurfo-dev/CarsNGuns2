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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Marker")
	FLinearColor MarkerColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly)
	class UWidgetComponent* MissionMarkerWidgetComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	EMissionState MissionState;

	UPROPERTY()
	class UMissionMarkerWidget* MissionMarkerWidget;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
