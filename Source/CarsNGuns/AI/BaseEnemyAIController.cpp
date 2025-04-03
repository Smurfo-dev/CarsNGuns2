// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyAIController.h"

#include "CarsNGuns/Systems/DefaultGameState.h"
#include "CarsNGuns/Systems/RoadGenerator.h"
#include "CarsNGuns/Systems/RoadManager.h"
#include "CarsNGuns/Vehicles/EnemyVehicleBase.h"
#include "CarsNGuns/Vehicles/PlayerVehicleBase.h"
#include "Components/SplineComponent.h"
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
	PlayerReference = Cast<APlayerVehicleBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	EnemyVehicleReference = Cast<AEnemyVehicleBase>(GetPawn());
	
	DefaultGameState = GetWorld()->GetGameState<ADefaultGameState>();

	PhysicsComponent = Cast<UPrimitiveComponent>(GetPawn()->GetRootComponent());
}

void ABaseEnemyAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(EnemyVehicleReference && EnemyVehicleReference->bDebugAI)
	{
		FString StateText = FString::Printf(TEXT("State: %s"), *GetEnemyStateAsString());
		DrawDebugString(GetWorld(), EnemyVehicleReference->GetActorLocation() + FVector(0, 0, 100.0f), StateText, nullptr, FColor::Blue, 0.0f, false, 4.0f);

		DrawDebugSphere(GetWorld(), PlayerReference->GetActorLocation(), BrakingRadius, 32, FColor::Red, false, -1.0f, 0, 2.0f);

		// Draw Follow Radius (Green)
		DrawDebugSphere(GetWorld(), PlayerReference->GetActorLocation(), FollowRadius, 32, FColor::Green, false, -1.0f, 0, 2.0f);

		//UE_LOG(LogTemp, Warning, TEXT("Follow State Time Elapsed: %f"), FollowStateTimeElapsed);
	}

	if(EnemyVehicleReference)
	{
		if (EnemyVehicleReference->bIsDead) SetState(EAIState::Dead);

		if (EnemyVehicleReference->bShouldIdle) SetState(EAIState::Idle);
	}

	if(!PlayerReference || PlayerReference->bIsDead)
	{
		SetState(EAIState::Idle);
	}
	
	switch (CurrentState)
	{
	case EAIState::Follow:
		Follow();
		
		FollowStateTimeElapsed += DeltaSeconds;

		if (FollowStateTimeElapsed >= FollowStateDuration)
		{
			TransitionFromFollow();
		}
		
		break;
	case EAIState::Chase:
		Chase();
		FollowStateTimeElapsed = 0.0f;
		break;
	case EAIState::Torpedo:
		Torpedo();
		FollowStateTimeElapsed = 0.0f;
		break;
	case EAIState::Shooting:
		Shooting();
		FollowStateTimeElapsed = 0.0f;
		break;
	case EAIState::Idle:
		Idle();
		FollowStateTimeElapsed = 0.0f;
		break;
	case EAIState::Dead:
		Dead();
		FollowStateTimeElapsed = 0.0f;
		break;
	}
	
}

void ABaseEnemyAIController::SetState(EAIState NewState)
{
	CurrentState = NewState;
}

void ABaseEnemyAIController::Follow()
{
	if(PlayerReference && DefaultGameState->GetRoadManager())
	{
		FVector EnemyLocation = GetPawn()->GetActorLocation();
		FVector PlayerLocation = PlayerReference->GetActorLocation();
		float DistanceToPlayer = FVector::Dist(EnemyLocation, PlayerLocation);

		USplineComponent* BestSpline = nullptr;
		float ClosestSplineDistance = FLT_MAX;
		float SplineKey = 0.0f;

		for (auto Road : DefaultGameState->GetRoadManager()->GetRoads())
		{
			if (!Road || !Road->GetSplineComponent()) continue;
			
			FVector ClosestPoint = Road->GetSplineComponent()->FindLocationClosestToWorldLocation(EnemyLocation, ESplineCoordinateSpace::World);
			float TempDistance = FVector::Dist(EnemyLocation, ClosestPoint);
			if (TempDistance < ClosestSplineDistance)
			{
				ClosestSplineDistance = TempDistance;
				BestSpline = Road->GetSplineComponent();
				SplineKey = BestSpline->FindInputKeyClosestToWorldLocation(EnemyLocation);
			}
		}

		if (!BestSpline)
		{
			UE_LOG(LogTemp, Warning, TEXT("No Best Spline Found, Returning..."));
			return;
		}

		float CurrentDistanceOnSpline = BestSpline->GetDistanceAlongSplineAtSplineInputKey(SplineKey);
		
		float LookAheadDistance = FMath::Clamp(EnemyVehicleReference->GetVelocity().Size() * 1.5f, 500.0f, 2000.0f);
		
		FVector TargetSplinePoint = BestSpline->GetLocationAtDistanceAlongSpline(CurrentDistanceOnSpline + LookAheadDistance, ESplineCoordinateSpace::World);
		
		FVector DirectionToSpline = (TargetSplinePoint - EnemyLocation).GetSafeNormal();
		
		if(PhysicsComponent)
		{
			FVector ForwardVector = GetPawn()->GetActorForwardVector();
			float AngleToSpline = FMath::Acos(FVector::DotProduct(ForwardVector, DirectionToSpline));
			float MaxSpeed = 150.0f;
			float MinSpeed = 20.0f;
			float SpeedMultiplier = 1.0f - FMath::Clamp(AngleToSpline, 0.1, 1.f);
			float TargetSpeed = MinSpeed + (MaxSpeed - MinSpeed) * SpeedMultiplier;

			UE_LOG(LogTemp, Warning, TEXT("Angle(Radians) To Spline: %f"), AngleToSpline);
			UE_LOG(LogTemp, Warning, TEXT("Speed Multiplier: %f"), SpeedMultiplier);
			UE_LOG(LogTemp, Warning, TEXT("TargetSpeed: %f"), TargetSpeed);

			float CurrentSpeed = EnemyVehicleReference->GetCurrentSpeed();
			float SpeedDifference = TargetSpeed - CurrentSpeed;

			float ThrottleInput = 0;
			float BrakeInput = 0;

			if (SpeedDifference > 50.0f)
			{
				ThrottleInput = FMath::Clamp(SpeedDifference / 150.0f, 0.1, 1.0f);
				BrakeInput = 0;
			}
			else if (SpeedDifference < -50.0f)
			{
				ThrottleInput = 0;
				BrakeInput = FMath::Clamp(FMath::Abs(SpeedDifference) / 100.0f, 0.f, 1.0f);
			}
			
			
			EnemyVehicleReference->SetThrottleInput(ThrottleInput);
			EnemyVehicleReference->SetBrakingInput(BrakeInput);

			UE_LOG(LogTemp, Warning, TEXT("Throttle Input: %f"), ThrottleInput);
			UE_LOG(LogTemp, Warning, TEXT("Braking Input: %f"), BrakeInput);

			//STEERING HANDLING

			FVector CrossProduct = FVector::CrossProduct(ForwardVector, DirectionToSpline);
			float SteeringDirection = (CrossProduct.Z > 0) ? 1.0f : -1.0f;

			float SteeringStrength = FMath::Clamp(FMath::Sin(AngleToSpline) * SteeringDirection * 0.5, -1.0f, 1.0f);
			
			const float DeadZoneThreshold = FMath::DegreesToRadians(10.0f);
			if (AngleToSpline < DeadZoneThreshold)
			{
				const float SnapBackTorque = 20000000.0f * EnemyVehicleReference->SnapBackTorqueFactor; // Adjust this value as needed for how quickly you want to snap back
				FVector AngularVelocity = PhysicsComponent->GetPhysicsAngularVelocityInRadians();
				const FVector NewSnapBackTorque = -AngularVelocity * SnapBackTorque;
				PhysicsComponent->AddTorqueInRadians(NewSnapBackTorque);
			}

			UE_LOG(LogTemp, Warning, TEXT("Final Steering Input: %f"), SteeringStrength);
			
			EnemyVehicleReference->SetSteeringInput(SteeringStrength);

			if (DistanceToPlayer > FollowRadius) SetState(EAIState::Chase);

			DrawDebugSphere(GetWorld(), TargetSplinePoint, 50.0f, 12, FColor::Green, false, 0.1f);
			
			DrawDebugLine(GetWorld(), EnemyLocation, TargetSplinePoint, FColor::Yellow, false, 0.1f, 0, 5.0f);
			
			FVector SteeringVector = ForwardVector.RotateAngleAxis(SteeringStrength * 45.0f, FVector(0, 0, 1)); // Simulated steering effect
			DrawDebugLine(GetWorld(), EnemyLocation, EnemyLocation + (SteeringVector * 300.0f), FColor::Red, false, 0.1f, 0, 5.0f);
		}
	}
}


void ABaseEnemyAIController::Chase()
{
	if(PlayerReference && DefaultGameState->GetRoadManager())
	{
		FVector EnemyLocation = GetPawn()->GetActorLocation();
		FVector PlayerLocation = PlayerReference->GetActorLocation();
		float DistanceToPlayer = FVector::Dist(EnemyLocation, PlayerLocation);

		USplineComponent* BestSpline = nullptr;
		float ClosestSplineDistance = FLT_MAX;
		float SplineKey = 0.0f;

		for (auto Road : DefaultGameState->GetRoadManager()->GetRoads())
		{
			if (!Road || !Road->GetSplineComponent()) continue;
			
			FVector ClosestPoint = Road->GetSplineComponent()->FindLocationClosestToWorldLocation(EnemyLocation, ESplineCoordinateSpace::World);
			float TempDistance = FVector::Dist(EnemyLocation, ClosestPoint);
			if (TempDistance < ClosestSplineDistance)
			{
				ClosestSplineDistance = TempDistance;
				BestSpline = Road->GetSplineComponent();
				SplineKey = BestSpline->FindInputKeyClosestToWorldLocation(EnemyLocation);
			}
		}

		if (!BestSpline)
		{
			UE_LOG(LogTemp, Warning, TEXT("No Best Spline Found, Returning..."));
			return;
		}

		float CurrentDistanceOnSpline = BestSpline->GetDistanceAlongSplineAtSplineInputKey(SplineKey);
		
		float LookAheadDistance = FMath::Clamp(EnemyVehicleReference->GetVelocity().Size() * 1.5f, 500.0f, 2000.0f);
		
		FVector TargetSplinePoint = BestSpline->GetLocationAtDistanceAlongSpline(CurrentDistanceOnSpline + LookAheadDistance, ESplineCoordinateSpace::World);
		
		FVector DirectionToSpline = (TargetSplinePoint - EnemyLocation).GetSafeNormal();
		
		if(PhysicsComponent)
		{
			FVector ForwardVector = GetPawn()->GetActorForwardVector();
			float AngleToSpline = FMath::Acos(FVector::DotProduct(ForwardVector, DirectionToSpline));
			float MaxSpeed = 200.0f;
			float MinSpeed = 20.0f;
			float SpeedMultiplier = 1.0f - FMath::Clamp(AngleToSpline, 0.1, 1.f);
			float TargetSpeed = MinSpeed + (MaxSpeed - MinSpeed) * SpeedMultiplier;

			UE_LOG(LogTemp, Warning, TEXT("Angle(Radians) To Spline: %f"), AngleToSpline);
			UE_LOG(LogTemp, Warning, TEXT("Speed Multiplier: %f"), SpeedMultiplier);
			UE_LOG(LogTemp, Warning, TEXT("TargetSpeed: %f"), TargetSpeed);

			float CurrentSpeed = EnemyVehicleReference->GetCurrentSpeed();
			float SpeedDifference = TargetSpeed - CurrentSpeed;

			float ThrottleInput = 0;
			float BrakeInput = 0;

			if (SpeedDifference > 50.0f)
			{
				ThrottleInput = FMath::Clamp(SpeedDifference / 150.0f, 0.1, 1.0f);
				BrakeInput = 0;
			}
			else if (SpeedDifference < -50.0f)
			{
				ThrottleInput = 0;
				BrakeInput = FMath::Clamp(FMath::Abs(SpeedDifference) / 150.0f, 0.3f, 1.0f);
			}
			
			
			EnemyVehicleReference->SetThrottleInput(ThrottleInput);
			EnemyVehicleReference->SetBrakingInput(BrakeInput);
			UE_LOG(LogTemp, Warning, TEXT("Throttle Input: %f"), ThrottleInput);
			UE_LOG(LogTemp, Warning, TEXT("Braking Input: %f"), BrakeInput);

			//STEERING HANDLING

			FVector CrossProduct = FVector::CrossProduct(ForwardVector, DirectionToSpline);
			float SteeringDirection = (CrossProduct.Z > 0) ? 1.0f : -1.0f;

			float SteeringStrength = FMath::Clamp(FMath::Sin(AngleToSpline) * SteeringDirection * 0.5, -1.0f, 1.0f);
			
			const float DeadZoneThreshold = FMath::DegreesToRadians(10.0f);
			if (AngleToSpline < DeadZoneThreshold)
			{
				const float SnapBackTorque = 20000000.0f * EnemyVehicleReference->SnapBackTorqueFactor; // Adjust this value as needed for how quickly you want to snap back
				FVector AngularVelocity = PhysicsComponent->GetPhysicsAngularVelocityInRadians();
				const FVector NewSnapBackTorque = -AngularVelocity * SnapBackTorque;
				PhysicsComponent->AddTorqueInRadians(NewSnapBackTorque);
			}

			UE_LOG(LogTemp, Warning, TEXT("Final Steering Input: %f"), SteeringStrength);
			
			EnemyVehicleReference->SetSteeringInput(SteeringStrength);
			
			if (DistanceToPlayer < FollowRadius) SetState(EAIState::Follow);

			DrawDebugSphere(GetWorld(), TargetSplinePoint, 50.0f, 12, FColor::Green, false, 0.1f);
			
			DrawDebugLine(GetWorld(), EnemyLocation, TargetSplinePoint, FColor::Yellow, false, 0.1f, 0, 5.0f);
			
			FVector SteeringVector = ForwardVector.RotateAngleAxis(SteeringStrength * 45.0f, FVector(0, 0, 1)); // Simulated steering effect
			DrawDebugLine(GetWorld(), EnemyLocation, EnemyLocation + (SteeringVector * 300.0f), FColor::Red, false, 0.1f, 0, 5.0f);
		}
	}
}

void ABaseEnemyAIController::Idle()
{
	//Do Nothing
	if (PlayerReference && !PlayerReference->bIsDead && !EnemyVehicleReference->bShouldIdle) SetState(EAIState::Chase);
}

void ABaseEnemyAIController::Dead()
{
	//Just be dead
}

void ABaseEnemyAIController::Torpedo()
{
	//Overridden by children
}

void ABaseEnemyAIController::Shooting()
{
	//Overridden by children
}

void ABaseEnemyAIController::TransitionFromFollow()
{
	//Overridden by children
}

FString ABaseEnemyAIController::GetEnemyStateAsString() const
{
	switch (CurrentState)
	{
	case EAIState::Follow: return TEXT("Follow");
	case EAIState::Chase: return TEXT("Chase");
	case EAIState::Torpedo: return TEXT("Torpedo");
	case EAIState::Shooting: return TEXT("Shooting");
	case EAIState::Dead: return TEXT("Dead");
	case EAIState::Idle: return TEXT("Idle");
	default: return TEXT("Unknown");
	}
}
