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

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABaseEnemyAIController::InitializeReferences);
}

void ABaseEnemyAIController::InitializeReferences()
{
	// Ensure player and enemy vehicle are set
	PlayerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Reference correctly found"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Player Reference not found!"));
	}

	EnemyVehicleReference = Cast<AEnemyVehicleBase>(GetPawn());
	if (EnemyVehicleReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Vehicle correctly found"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Enemy Vehicle not found!"));
	}
}

void ABaseEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (EnemyVehicleReference)
	{
		if (EnemyVehicleReference->bDebugAI)
		{
			FString StateText = FString::Printf(TEXT("State: %s"), *GetEnemyStateAsString());
			DrawDebugString(GetWorld(), EnemyVehicleReference->GetActorLocation() + FVector(0, 0, 100.0f), StateText, nullptr, FColor::Blue, 0.0f, false, 4.0f);

			DrawDebugSphere(GetWorld(), PlayerReference->GetActorLocation(), BrakingRadius, 32, FColor::Red, false, -1.0f, 0, 2.0f);

			// Draw Follow Radius (Green)
			DrawDebugSphere(GetWorld(), PlayerReference->GetActorLocation(), FollowRadius, 32, FColor::Green, false, -1.0f, 0, 2.0f);
		}
	}
	
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

			FVector CrossProduct = FVector::CrossProduct(ForwardVector, DirectionToPlayer);
			float SteeringDirection = (CrossProduct.Z > 0) ? 1.0f : -1.0f;

			const float DeadZoneThreshold = FMath::DegreesToRadians(10.0f);
			if (AngleToPLayer < DeadZoneThreshold)
			{
				EnemyVehicleReference->SetSteeringInput(0);
				return;
			}


			float SteeringStrength = FMath::Clamp(FMath::Sin(AngleToPLayer) * SteeringDirection * 0.5, -1.0f, 1.0f);

			static float PreviousSteeringInput = 0.0f;
			float DampingFactor = 0.9f; //Adjust for stability (0.8-0.95 recommended)
			
			float SmoothedSteeringInput = FMath::Lerp(PreviousSteeringInput, SteeringStrength, 1.0f - DampingFactor);
			PreviousSteeringInput = SmoothedSteeringInput;
			
			EnemyVehicleReference->SetSteeringInput(SmoothedSteeringInput);

			if(DistanceToPlayer < BrakingRadius)
			{
				EnemyVehicleReference->SetThrottleInput(0);
				EnemyVehicleReference->SetBrakingInput(1);

				UE_LOG(LogTemp, Warning, TEXT("Braking"));
			}
			else if(DistanceToPlayer > BrakingRadius && DistanceToPlayer < FollowRadius)
			{
				EnemyVehicleReference->SetThrottleInput(1);
				EnemyVehicleReference->SetBrakingInput(0);

				UE_LOG(LogTemp, Warning, TEXT("following"));
			}
			else if (DistanceToPlayer > FollowRadius)
			{
				SetState(EAIState::Chase);
			}
			
		}
	}
}

void ABaseEnemyAIController::Chase()
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

			FVector CrossProduct = FVector::CrossProduct(ForwardVector, DirectionToPlayer);
			float SteeringDirection = (CrossProduct.Z > 0) ? 1.0f : -1.0f;

			const float DeadZoneThreshold = FMath::DegreesToRadians(10.0f);
			if (AngleToPLayer < DeadZoneThreshold)
			{
				EnemyVehicleReference->SetSteeringInput(0);
				return;
			}
			
			float SteeringStrength = FMath::Clamp(FMath::Sin(AngleToPLayer) * SteeringDirection * 0.5, -1.0f, 1.0f);

			static float PreviousSteeringInput = 0.0f;
			float DampingFactor = 0.9f; //Adjust for stability (0.8-0.95 recommended)
			
			float SmoothedSteeringInput = FMath::Lerp(PreviousSteeringInput, SteeringStrength, 1.0f - DampingFactor);
			PreviousSteeringInput = SmoothedSteeringInput;
			
			EnemyVehicleReference->SetSteeringInput(SmoothedSteeringInput);

			if (DistanceToPlayer < FollowRadius) SetState(EAIState::Follow);

			EnemyVehicleReference->SetThrottleInput(5);
			EnemyVehicleReference->SetBrakingInput(0);
		}
	}
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
