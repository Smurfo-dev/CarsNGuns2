// Fill out your copyright notice in the Description page of Project Settings.


#include "TorpedoAIController.h"
#include "EnemyVehicleBase.h"

void ATorpedoAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATorpedoAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (EnemyVehicleReference)
	{
		if (EnemyVehicleReference->bDebugAI)
		{
			FVector TextLocation = EnemyVehicleReference->GetActorLocation() + FVector(0, 0, 200.0f); // 200 units above the enemy
			FString DebugText = TEXT("TORPEDO AI");
			FColor TextColor = FColor::Red;
			float Duration = 0.0f; // 0 means persistent until next frame

			// Draw the debug text
			DrawDebugString(GetWorld(), TextLocation, DebugText, nullptr, TextColor, Duration, false, 5.0f);
		}
	}
}
void ATorpedoAIController::Torpedo()
{
	Super::Torpedo();

	Follow();
	//Torpedo Code
}

void ATorpedoAIController::TransitionFromFollow()
{
	Super::TransitionFromFollow();

	SetState(EAIState::Torpedo);
}
