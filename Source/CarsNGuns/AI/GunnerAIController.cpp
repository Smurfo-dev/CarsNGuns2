// Fill out your copyright notice in the Description page of Project Settings.


#include "GunnerAIController.h"

#include "CarsNGuns/Weapons/BaseWeapon.h"
#include "CarsNGuns/Vehicles/EnemyVehicleBase.h"

void AGunnerAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AGunnerAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentState != EAIState::Shooting && bIsFiring)
	{
		EnemyVehicleReference->GetPrimaryWeapon()->StopFire();
		bIsFiring = false;
	}

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

void AGunnerAIController::Shooting()
{
	Super::Shooting();

	Follow();

	if (EnemyVehicleReference->GetPrimaryWeapon())
	{
		EnemyVehicleReference->GetPrimaryWeapon()->Fire();
		bIsFiring = true;
	}
}

void AGunnerAIController::TransitionFromFollow()
{
	Super::TransitionFromFollow();

	SetState(EAIState::Shooting);
}
