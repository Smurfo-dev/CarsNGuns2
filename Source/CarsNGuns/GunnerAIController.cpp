// Fill out your copyright notice in the Description page of Project Settings.


#include "GunnerAIController.h"
#include "EnemyVehicleBase.h"

void AGunnerAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AGunnerAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (EnemyVehicleReference)
	{
		if (EnemyVehicleReference->bDebugAI)
		{
			FVector TextLocation = EnemyVehicleReference->GetActorLocation() + FVector(0, 0, 200.0f); // 200 units above the enemy
			FString DebugText = TEXT("GUNNER AI");
			FColor TextColor = FColor::Red;
			float Duration = 0.0f; // 0 means persistent until next frame

			// Draw the debug text
			DrawDebugString(GetWorld(), TextLocation, DebugText, nullptr, TextColor, Duration, false, 5.0f);
		}
	}
	
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
