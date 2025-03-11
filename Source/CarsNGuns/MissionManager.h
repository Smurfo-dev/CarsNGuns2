// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissionManager.generated.h"

UCLASS()
class CARSNGUNS_API AMissionManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissionManager();

	UFUNCTION(BlueprintCallable)
	void StartEvent(TSubclassOf<class ABaseMission> MissionType, FVector Location);

	UFUNCTION(BlueprintCallable)
	void EndEvent(ABaseMission* Mission);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	TArray<ABaseMission*> ActiveMissions;

};
