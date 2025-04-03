// Fill out your copyright notice in the Description page of Project Settings.


#include "CarsNGuns/Systems/RoadManager.h"
#include "RoadGenerator.h"

// Sets default values
ARoadManager::ARoadManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ARoadManager::AddRoad(AActor* NewRoad)
{
	ARoadGenerator* Road = Cast<ARoadGenerator>(NewRoad);
	if (Road && !Roads.Contains(Road))
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Road"));
		Roads.Add(Road);
	}
}

void ARoadManager::ClearRoadArray()
{
	if(!Roads.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Clearing Road Array"));
		Roads.Empty();
	}
}

bool ARoadManager::FindClosestPointOnSpline(const FVector& TargetLocation, FVector& OutClosestPoint)
{
	float ClosestDistance = FLT_MAX;
	FVector BestPoint;
	for (const auto Road : Roads)
	{
		const float SplineKey = Road->GetSplineComponent()->FindInputKeyClosestToWorldLocation(TargetLocation);
		FVector SplinePoint = Road->GetSplineComponent()->GetLocationAtSplineInputKey(SplineKey, ESplineCoordinateSpace::World);

		float Distance = FVector::Dist(TargetLocation, SplinePoint);
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			BestPoint = SplinePoint;
		}
	}

	if (ClosestDistance < 10000.0f)
	{
		OutClosestPoint = BestPoint;
		return true;
	}
	return false;
}

FRotator ARoadManager::GetRoadDirectionAtPoint(const FVector& Point)
{
	for (auto Road : Roads)
	{
		float SplineKey = Road->GetSplineComponent()->FindInputKeyClosestToWorldLocation(Point);
		
		return Road->GetSplineComponent()->GetRotationAtSplineInputKey(SplineKey, ESplineCoordinateSpace::World);
	}
	
	return FRotator::ZeroRotator;
}

// Called when the game starts or when spawned
void ARoadManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoadManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

