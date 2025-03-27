// Fill out your copyright notice in the Description page of Project Settings.


#include "CarsNGuns/Systems/RoadManager.h"

// Sets default values
ARoadManager::ARoadManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ARoadManager::AddRoad(AActor* NewRoad)
{
	if (NewRoad && !Roads.Contains(NewRoad))
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Road"));
		Roads.Add(NewRoad);
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

