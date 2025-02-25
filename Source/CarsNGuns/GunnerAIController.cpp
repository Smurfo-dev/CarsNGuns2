// Fill out your copyright notice in the Description page of Project Settings.


#include "GunnerAIController.h"

void AGunnerAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AGunnerAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AGunnerAIController::Follow()
{
	Super::Follow();

	//Edit follow to transition to shooting
}

void AGunnerAIController::Shooting()
{
	Super::Shooting();

	//shooting code
}
