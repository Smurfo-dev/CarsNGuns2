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
			float SteeringInput = (CrossProduct.Z > 0) ? 1.0f : -1.0f;
			FMath::Clamp(SteeringInput*=AngleToPLayer/2, -1, 1);

			UE_LOG(LogTemp, Warning, TEXT("Final Steering Input: %f"), SteeringInput);
			
			EnemyVehicleReference->SetSteeringInput(SteeringInput);

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
