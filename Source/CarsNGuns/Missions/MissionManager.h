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
	void StartEvent(class ABaseMission* Mission);

	UFUNCTION(BlueprintCallable)
	void EndEvent(ABaseMission* Mission, bool bSuccess);

	void AddMission(ABaseMission* Mission);

	void SetPlayerVehicleReference(class ABasePhysicsVehiclePawn* PlayerVehicle);

	UFUNCTION(BlueprintCallable)
	TArray<ABaseMission*> GetMissions()
	{
		return Missions;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	TArray<ABaseMission*> Missions;

	UPROPERTY()
	ABasePhysicsVehiclePawn* PlayerVehicleReference;

	UFUNCTION()
	void GrantRewards();

};
