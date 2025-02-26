// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyAIController.h"
#include "EnemyVehicleBase.h"
#include "Kismet/GameplayStatics.h"

ABaseEnemyAIController::ABaseEnemyAIController()
{
	CurrentState = EAIState::Follow;
}

void ABaseEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	PlayerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if(PlayerReference) UE_LOG(LogTemp, Warning, TEXT("Player Reference correctly found"));

	EnemyVehicleReference = Cast<AEnemyVehicleBase>(GetPawn());
	if(EnemyVehicleReference) UE_LOG(LogTemp, Warning, TEXT("Enemy Vehicle correctly found"));
}

void ABaseEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FString StateText = FString::Printf(TEXT("State: %s"), *GetEnemyStateAsString());
	DrawDebugString(GetWorld(), EnemyVehicleReference->GetActorLocation() + FVector(0, 0, 100.0f), StateText, nullptr, FColor::Blue, 0.0f, false, 4.0f);

	switch (CurrentState)
	{
	case EAIState::Follow:
		Follow();
		break;
	case EAIState::Chase:
		Chase();
		break;
	case EAIState::Torpedo:
		Torpedo();
		break;
	case EAIState::Shooting:
		Shooting();
		break;
	}
	
}

void ABaseEnemyAIController::SetState(EAIState NewState)
{
	CurrentState = NewState;
}

void ABaseEnemyAIController::Follow()
{
	if(PlayerReference)
	{
		FVector EnemyLocation = GetPawn()->GetActorLocation();
		FVector PlayerLocation = PlayerReference->GetActorLocation();

		FVector DirectionToPlayer = (PlayerLocation - EnemyLocation).GetSafeNormal();
		float DistanceToPlayer = FVector::Dist(EnemyLocation, PlayerLocation);
		UE_LOG(LogTemp, Warning, TEXT("Distance to Player: %f"), DistanceToPlayer);

		UPrimitiveComponent* PhysicsComponent = Cast<UPrimitiveComponent>(GetPawn()->GetRootComponent());
		
		if(PhysicsComponent)
		{
			FVector ForwardVector = GetPawn()->GetActorForwardVector();

			float AngleToPLayer = FMath::Acos(FVector::DotProduct(ForwardVector, DirectionToPlayer));
			UE_LOG(LogTemp, Warning, TEXT("Angle to Player: %f"), AngleToPLayer);

			FVector CrossProduct = FVector::CrossProduct(ForwardVector, DirectionToPlayer);
			float SteeringDirection = (CrossProduct.Z > 0) ? 1.0f : -1.0f;

			const float DeadZoneThreshold = FMath::DegreesToRadians(10.0f);
			if (AngleToPLayer < DeadZoneThreshold)
			{
				EnemyVehicleReference->SetSteeringInput(0);
				UE_LOG(LogTemp, Warning, TEXT("Angle Deadzone Reached"));
				return;
			}


			float SteeringStrength = FMath::Clamp(FMath::Sin(AngleToPLayer) * SteeringDirection * 0.5, -1.0f, 1.0f);

			static float PreviousSteeringInput = 0.0f;
			float DampingFactor = 0.9f; //Adjust for stability (0.8-0.95 recommended)
			
			float SmoothedSteeringInput = FMath::Lerp(PreviousSteeringInput, SteeringStrength, 1.0f - DampingFactor);
			PreviousSteeringInput = SmoothedSteeringInput;

			UE_LOG(LogTemp, Warning, TEXT("Final Steering Input: %f"), SmoothedSteeringInput);
			
			EnemyVehicleReference->SetSteeringInput(SmoothedSteeringInput);

			if(DistanceToPlayer > TrackingRadius)
			{
				EnemyVehicleReference->SetBrakingInput(0);
				EnemyVehicleReference->SetThrottleInput(1);
			}
			else
			{
				EnemyVehicleReference->SetThrottleInput(0);
				EnemyVehicleReference->SetBrakingInput(1);
			}
			
		}
	}
}

void ABaseEnemyAIController::Chase()
{
	
}

void ABaseEnemyAIController::Torpedo()
{
	
}

void ABaseEnemyAIController::Shooting()
{
	
}

FString ABaseEnemyAIController::GetEnemyStateAsString() const
{
	switch (CurrentState)
	{
	case EAIState::Follow: return TEXT("Follow");
	case EAIState::Chase: return TEXT("Chase");
	case EAIState::Torpedo: return TEXT("Torpedo");
	case EAIState::Shooting: return TEXT("Shooting");
	default: return TEXT("Unknown");
	}
}
