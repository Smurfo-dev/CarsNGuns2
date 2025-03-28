// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoadManager.generated.h"

UCLASS()
class CARSNGUNS_API ARoadManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoadManager();

	void AddRoad(AActor* NewRoad);

	void ClearRoadArray();
	
	// Get the list of enemies.
	const TArray<class ARoadGenerator*>& GetRoads() const { return Roads; }

	bool FindClosestPointOnSpline(const FVector& TargetLocation, FVector& OutClosestPoint);

	FRotator GetRoadDirectionAtPoint(const FVector& Point);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	TArray<ARoadGenerator*> Roads;

};
