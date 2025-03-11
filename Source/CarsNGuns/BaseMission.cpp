// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMission.h"

// Sets default values
ABaseMission::ABaseMission()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseMission::StartEvent()
{
	
}

void ABaseMission::EndEvent(bool bSuccess)
{
	
}

// Called when the game starts or when spawned
void ABaseMission::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseMission::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

