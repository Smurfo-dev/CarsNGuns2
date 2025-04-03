// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePhysicsVehiclePawn.h"

#include "CarsNGuns/Weapons/BaseWeapon.h"
#include "CarsNGuns/Components/HealthComponent.h"
#include "CarsNGuns/Components/UpgradeHandlerComponent.h"
#include "CarsNGuns/Player/MyPlayerController.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABasePhysicsVehiclePawn::ABasePhysicsVehiclePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create primitive and set as root
	CollisionBoxPrimitive = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxPrimitive"));
	RootComponent = CollisionBoxPrimitive;

	//Init box size
	CollisionBoxPrimitive->InitBoxExtent(FVector(125.0f, 100.0f, 50.0f));

	//Enable physics
	CollisionBoxPrimitive->SetSimulatePhysics(true);

	//Enable collision
	CollisionBoxPrimitive->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBoxPrimitive->SetCollisionObjectType(ECC_PhysicsBody);

	VehicleMeshComponent = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("VehicleMeshComponent"));
	VehicleMeshComponent->SetupAttachment(RootComponent);

	//Create Health Component
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	//Default Armor Level
	ArmorLevel = EArmorLevel::None;

	//Create Upgrade Handler
	UpgradeHandlerComponent =  CreateDefaultSubobject<UUpgradeHandlerComponent>(TEXT("UpgradeHandlerComponent"));

	//Create Audio Component(s)
	EngineAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudioComponent"));

	//Initialize Wheel Config
	WheelConfig.WheelType = EWheelType::Comfort;
	WheelConfig.DisplayName = FName("Comfort Wheels");
	WheelConfig.TarmacGripMultiplier = 1.0f;
	WheelConfig.LightOffRoadGripMultiplier = 0.9f;
	WheelConfig.HeavyOffRoadGripMultiplier = 0.75f;
	WheelConfig.TarmacSlowDownFactor = 1.0f;
	WheelConfig.LightOffRoadSlowDownFactor = 0.8f;
	WheelConfig.HeavyOffRoadSlowDownFactor = 0.6f;
	
}

// Called when the game starts or when spawned
void ABasePhysicsVehiclePawn::BeginPlay()
{
	Super::BeginPlay();

	InitAudio();
	
	//Attach chosen weapon classes to player vehicle
	//Weapon ID must be the same as corresponding socket names

	if (bAutoEquipWeapons)
	{
		if (PrimaryWeaponClass) AttachPrimaryWeaponToVehicle(PrimaryWeaponClass);
		if (SecondaryWeaponClass) AttachSecondaryWeaponToVehicle(SecondaryWeaponClass);
	}

	UpgradeHandlerComponent->OwnerReference = this;

	CurrentSteeringInput = 0.0f;
	
}

// Called every frame
void ABasePhysicsVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bIsDead)
	{
		IsInAir();

		if(bIsInAir)
		{
			CollisionBoxPrimitive->SetCenterOfMass(FVector(0,0,-50));
			CollisionBoxPrimitive->SetMassScale(NAME_None, 5);
		}
		else
		{
			CollisionBoxPrimitive->SetCenterOfMass(FVector(0,0,0));
			CollisionBoxPrimitive->SetMassScale(NAME_None, 1);
		}

		//Record Wheel Positions
		WheelPositions.Empty();
		WheelPositions.Add(VehicleMeshComponent->GetSocketLocation(TEXT("Wheel_Front_Left_Locator")));
		WheelPositions.Add(VehicleMeshComponent->GetSocketLocation(TEXT("Wheel_Front_Right_Locator")));
		WheelPositions.Add(VehicleMeshComponent->GetSocketLocation(TEXT("Wheel_Rear_Left_Locator")));
		WheelPositions.Add(VehicleMeshComponent->GetSocketLocation(TEXT("Wheel_Rear_Right_Locator")));
		
		//Updates engine data
		UpdateEngine();
	
		//Suspension Force Calculations
		CalculateSuspensionForces();

		//Applies Steering Forces
		ApplySteeringTorque();

		//Apply Throttle Forces
		ApplyThrottle();

		//Apply Handbrake Forces
		ApplyHandbrake();

		//Apply Braking Forces
		ApplyBraking();

		//Applies Friction Forces
		ApplyFrictionForces();

		//Apply Downforce
		ApplyDownforce();

		//Traction Force Calculations
		CalculateTractionForces();
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Is Grounded: %d"), IsGrounded());

	//Animate wheels
	UpdateWheelAnimations(DeltaTime);

	//Update Engine SFX
	UpdateEngineSFX();

}

void ABasePhysicsVehiclePawn::CalculateSuspensionForces()
{
	if(CollisionBoxPrimitive)
	{
		int Counter = 0;
		for(FVector& TraceStart : WheelPositions)
		{
			FVector TraceEnd = TraceStart + FVector(0.0f, 0.0f, -MaxSuspensionExtent);
			FHitResult HitResult;

			FCollisionQueryParams Params;
			Params.bReturnPhysicalMaterial = true;
			Params.AddIgnoredActor(this); //Raycast ignores the actor itself

			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);
			
			if(bDebugForces) DrawDebugLine(GetWorld(), TraceStart, TraceEnd, bHit ? FColor::Green : FColor::Red, false, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()), 0, 1);

			if(bHit)
			{
				const float CompressionRatio = 1 - HitResult.Distance / MaxSuspensionExtent;
				
				FString CompressionText = FString::Printf(TEXT("Compression Ratio: %.2f"), CompressionRatio);
				if(bDebugForces) DrawDebugString(GetWorld(), TraceStart+FVector(0.0f, 0.0f, -10.0f), CompressionText, nullptr, FColor::White, 0.0f, true);
				
				const float ForceToApply = CompressionRatio * (SuspensionForceFactor*1000000) - (SuspensionDampingFactor*1000) * CollisionBoxPrimitive->GetPhysicsLinearVelocityAtPoint(TraceStart).Z;
				CollisionBoxPrimitive->AddForceAtLocation(FVector(0.0f, 0.0f, ForceToApply), TraceStart, NAME_None);

				WheelGroundPositions[WheelPositions.Find(TraceStart)] = true;

				RoadNormal = HitResult.Normal;

				SuspensionHeights[Counter] = MaxSuspensionExtent - MaxSuspensionExtent * CompressionRatio;

				if(HitResult.PhysMaterial.IsValid()) CurrentSurfaceMaterials[WheelPositions.Find(TraceStart)] = HitResult.PhysMaterial.Get();
			}
			else
			{
				WheelGroundPositions[WheelPositions.Find(TraceStart)] = false;
			}
			Counter++;
		}
	}
}

void ABasePhysicsVehiclePawn::CalculateTractionForces()
{
	if(!bIsInAir)
	{
		if(bDebugDrifting)
		{
			FString SlideString = FString::Printf(TEXT("Is Sliding: %d"), FMath::Abs(CalculateDriftAngle()) > 25.0f && CurrentSpeed > 20.0f);
			DrawDebugString(GetWorld(), CollisionBoxPrimitive->GetComponentLocation() + FVector(0, -25, 100), SlideString, nullptr, FColor::White, 0.0f, true);
			FString DriftString = FString::Printf(TEXT("Is Drifting: %d"), bIsDrifting);
			DrawDebugString(GetWorld(), CollisionBoxPrimitive->GetComponentLocation() + FVector(0, -25, 90), DriftString, nullptr, FColor::White, 0.0f, true);
			FString AngleString = FString::Printf(TEXT("Drift Angle: %i"), FMath::RoundToInt(CalculateDriftAngle()));
			DrawDebugString(GetWorld(), CollisionBoxPrimitive->GetComponentLocation() + FVector(0, -25, 80), AngleString, nullptr, FColor::White, 0.0f, true);
		}
	
		if(bIsDrifting)
		{
			float DriftAngle = CalculateDriftAngle();
		
			float DriftAngleFactor = FMath::Clamp(FMath::Abs(DriftAngle) / MaxDriftAngle, 0.0f, 1.0f);
			float CorrectiveForceMagnitude = DriftAngleFactor * 10000 * MaxCorrectiveForwardForceFactor;
			FVector CorrectiveForce = CollisionBoxPrimitive->GetForwardVector() * CorrectiveForceMagnitude;
			CorrectiveForce += CollisionBoxPrimitive->GetForwardVector() * (ThrottleInput * ThrottleDriftCorrectionBoost);

			CollisionBoxPrimitive->AddForce(CorrectiveForce, NAME_None, false);
			// Debug visualization
			if (bDebugForces)
			{
				DrawDebugDirectionalArrow(
					GetWorld(),
					CollisionBoxPrimitive->GetComponentLocation(),
					CollisionBoxPrimitive->GetComponentLocation() + CorrectiveForce * 0.001f,
					100.0f, 
					FColor::Cyan,
					false, 
					-1.0f, 
					0, 
					3.0f
				);
			}
		
			if(FMath::Abs(DriftAngle) < 20.0f) bIsDrifting = false;
		
		}
	}
	
}

void ABasePhysicsVehiclePawn::ApplySteeringTorque()
{
	if(!bIsInAir)
	{
		if(WheelGroundPositions[0] && WheelGroundPositions[1])
		{
			const float SteeringSensitivity = SteeringSensitivityCurve->GetFloatValue(CurrentSpeed) * SteeringSensitivityFactor;
			if(FMath::Abs(CurrentSpeed) > SpeedThreshold)
			{
				float SpeedBasedSteeringTorque;
				if(bIsDrifting) SpeedBasedSteeringTorque = CurrentSteeringInput * (SteeringTorqueForceFactor * 100000.0f / 2) * CurrentSpeed * SteeringSpeedAmplificationFactor * SteeringSensitivity * DriftSteeringSensitivityFactor;
				else SpeedBasedSteeringTorque = CurrentSteeringInput * (SteeringTorqueForceFactor * 100000.0f / 2) * CurrentSpeed * SteeringSpeedAmplificationFactor * SteeringSensitivity;

				//reverse steering torque if going backwards
				if(CurrentForwardSpeed < 0.0f && bIsReversing) SpeedBasedSteeringTorque = -SpeedBasedSteeringTorque;
				FVector Torque = FVector(0.0f, 0.0f, SpeedBasedSteeringTorque);
				CollisionBoxPrimitive->AddTorqueInRadians(Torque, TEXT("Wheel_Front_Left_Locator"));
				CollisionBoxPrimitive->AddTorqueInRadians(Torque, TEXT("Wheel_Front_Right_Locator"));

				if(bDebugForces)
				{
					FVector FrontLeftLocation = VehicleMeshComponent->GetSocketLocation(TEXT("Wheel_Front_Left_Locator"));
					FVector FrontRightLocation = VehicleMeshComponent->GetSocketLocation(TEXT("Wheel_Front_Right_Locator"));
					FRotator Rotation = FRotator(0,0,90) + GetActorRotation();
	 				FVector TorqueDirection = Rotation.RotateVector(Torque);
					TorqueDirection = TorqueDirection.GetSafeNormal();
				
					DrawDebugDirectionalArrow(
						GetWorld(),
						FrontLeftLocation,
						FrontLeftLocation + TorqueDirection * 200.0f * FMath::Abs(CurrentSteeringInput), // Scale the torque vector for visualization
						20.0f, // Arrow size
						FColor::Red, // Color of the arrow
						false, // Whether to persist
						-1.0f, // Lifetime, -1 for permanent
						0, // Depth priority
						5.0f // Thickness
					);

					DrawDebugDirectionalArrow(
						GetWorld(),
						FrontRightLocation,
						FrontRightLocation + TorqueDirection * 200.0f * FMath::Abs(CurrentSteeringInput), // Scale the torque vector for visualization
						20.0f, // Arrow size
						FColor::Blue, // Color of the arrow
						false, // Whether to persist
						-1.0f, // Lifetime, -1 for permanent
						0, // Depth priority
						5.0f // Thickness
					);
				}
				
			}
			else
			{
				CollisionBoxPrimitive->AddTorqueInRadians(FVector::ZeroVector);
			}
		}
		
		//Steering Torque Damping
		const float TorqueDamping = SteeringTorqueDampingFactor * 1000000.0f;
		FVector AngularVelocity = CollisionBoxPrimitive->GetPhysicsAngularVelocityInRadians();
		const FVector DampingTorque = -AngularVelocity * TorqueDamping;

		CollisionBoxPrimitive->AddTorqueInRadians(DampingTorque);
		

		// Only apply snap-back torque when there is no current steering input
		if (IsGrounded() && FMath::IsNearlyZero(CurrentSteeringInput) || (IsGrounded() && CurrentSpeed < RotationSpeedThreshold))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Snapback"));
			const float SnapBackTorque = 20000000.0f * SnapBackTorqueFactor; // Adjust this value as needed for how quickly you want to snap back
			AngularVelocity = CollisionBoxPrimitive->GetPhysicsAngularVelocityInRadians();
			const FVector NewSnapBackTorque = -AngularVelocity * SnapBackTorque;
			CollisionBoxPrimitive->AddTorqueInRadians(NewSnapBackTorque);
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Not Snapback"));
		}
	}
	
	
}

void ABasePhysicsVehiclePawn::ApplyThrottle()
{
	if(ThrottleInput == 0) return;
	if(!bIsShifting && !bIsInAir)
	{
		float SlowdownFactor = 0;
		for(const UPhysicalMaterial* Surface : CurrentSurfaceMaterials)
		{
			if(!Surface) return;
			switch (Surface->SurfaceType)
			{
			case SurfaceType1: //Tarmac
				SlowdownFactor += WheelConfig.TarmacSlowDownFactor;
				break;
			case SurfaceType2: //Gravel
				SlowdownFactor += WheelConfig.LightOffRoadSlowDownFactor;
				break;
			case SurfaceType3: //Snow
				SlowdownFactor += WheelConfig.LightOffRoadSlowDownFactor;
				break;
			case SurfaceType4: //Mud
				SlowdownFactor += WheelConfig.HeavyOffRoadSlowDownFactor;
				break;
			default: //Shouldnt really ever be initialized
				UE_LOG(LogTemp, Error, TEXT("No Correctly assigned Surface Material for Throttle effects"));
				SlowdownFactor += 1.0f;
				break;
			}
		}
		SlowdownFactor/=4;
		
		//Project forward vector to surface
		const FVector ForwardDirection = GetActorForwardVector();
		FVector ProjectedForward = FVector::VectorPlaneProject(ForwardDirection, RoadNormal);
		ProjectedForward.Normalize();

		for(int i = 0; i < 4; i++)
		{
			if(WheelGroundPositions[i])
			{
				if(!bIsRedlining || CurrentForwardSpeed < 0)
				{
					if(bIsReversing) bIsReversing = false;
					//Calculate power distribution
					float CalculatedThrottleForceFactor;
					if(i == 0 || i == 1) CalculatedThrottleForceFactor = ThrottleForceFactor * DifferentialRatio;
					else CalculatedThrottleForceFactor = ThrottleForceFactor * (1-DifferentialRatio);
					CollisionBoxPrimitive->AddForceAtLocation(ProjectedForward * CalculatedThrottleForceFactor * 50000 * GearboxConfig[CurrentGear - 1].AccelerationFactor * ThrottleInput * SlowdownFactor, WheelPositions[i]);
					if(bDebugForces)
					{
						DrawDebugSphere(GetWorld(), WheelPositions[i], 5.0f, 12, FColor::Blue, false, -1.0f);
						DrawDebugDirectionalArrow(GetWorld(), WheelPositions[i], WheelPositions[i] + ProjectedForward * CalculatedThrottleForceFactor * 100.0f, 50, FColor::Blue, false, -1.0f, 0, 5.0f);
					}
				}
			}
		}
	}
}

void ABasePhysicsVehiclePawn::ApplyBraking()
{
	if(BrakingInput == 0) return;
	bIsDrifting = false;
	if(!bIsShifting && !bIsInAir)
	{

		float SlowdownFactor = 0;
		for(const UPhysicalMaterial* Surface : CurrentSurfaceMaterials)
		{
			if(!Surface) return;
			switch (Surface->SurfaceType)
			{
			case SurfaceType1: //Tarmac
				SlowdownFactor += WheelConfig.TarmacSlowDownFactor;
				break;
			case SurfaceType2: //Gravel
				SlowdownFactor += WheelConfig.LightOffRoadSlowDownFactor;
				break;
			case SurfaceType3: //Snow
				SlowdownFactor += WheelConfig.LightOffRoadSlowDownFactor;
				break;
			case SurfaceType4: //Mud
				SlowdownFactor += WheelConfig.HeavyOffRoadSlowDownFactor;
				break;
			default: //Shouldnt really ever be initialized
				UE_LOG(LogTemp, Error, TEXT("No Correctly assigned Surface Material for Braking effects"));
				SlowdownFactor += 1.0f;
				break;
			}
		}

		SlowdownFactor/=4;
		
		//Project forward vector to surface
		const FVector ForwardDirection = GetActorForwardVector();
		FVector ProjectedForward = FVector::VectorPlaneProject(ForwardDirection, RoadNormal);
		ProjectedForward.Normalize();
		
		for(int i = 0; i < 4; i++)
		{
			if(WheelGroundPositions[i])
			{
				//Calculate power distribution
				float CalculatedReverseForceFactor;
				if(i == 0 || i == 1) CalculatedReverseForceFactor = ReverseForceFactor * DifferentialRatio;
				else CalculatedReverseForceFactor = ThrottleForceFactor * (1-DifferentialRatio);
				
				if(CurrentForwardSpeed < 0.0f && !bIsRedlining) // Reversing
				{
					bIsReversing = true;
					CollisionBoxPrimitive->AddForceAtLocation(-ProjectedForward * (CalculatedReverseForceFactor * 500000.0f) * BrakingInput * SlowdownFactor, WheelPositions[i]);
					if(bDebugForces)
					{
						DrawDebugSphere(GetWorld(), WheelPositions[i], 5.0f, 12, FColor::Blue, false, -1.0f);
						DrawDebugDirectionalArrow(GetWorld(), WheelPositions[i], WheelPositions[i] + -ProjectedForward * CalculatedReverseForceFactor * 100.0f, 50, FColor::Blue, false, -1.0f, 0, 5.0f);
					}
				}
				else if(!bIsRedlining) //Braking
				{
					CollisionBoxPrimitive->AddForceAtLocation(-ProjectedForward * (BrakeForceFactor * 50000.0f) * BrakingInput, WheelPositions[i]);
					if(bDebugForces)
					{
						DrawDebugSphere(GetWorld(), WheelPositions[i], 5.0f, 12, FColor::Blue, false, -1.0f);
						DrawDebugDirectionalArrow(GetWorld(), WheelPositions[i], WheelPositions[i] + -ProjectedForward * BrakeForceFactor * 25.0f, 50, FColor::Blue, false, -1.0f, 0, 5.0f);
					}
				}
			}
		}
	}
}

void ABasePhysicsVehiclePawn::ApplyHandbrake()
{
	if(bHandbrake)
	{
		if(!bLockUpVehicle)
		{
			FVector Velocity = CollisionBoxPrimitive->GetComponentVelocity();
			FVector ForwardVector = CollisionBoxPrimitive->GetForwardVector();
			float ForwardSpeed = FVector::DotProduct(Velocity, ForwardVector);

			if(ForwardSpeed > 0.0f)
			{
				FVector HandbrakeForce = -ForwardVector * 1000000.0f * HandbrakeForceFactor;
				CollisionBoxPrimitive->AddForce(HandbrakeForce);

				FVector NewVelocity = CollisionBoxPrimitive->GetComponentVelocity();
				if(FVector::DotProduct(NewVelocity, ForwardVector) < 0.0f)
				{
					NewVelocity -= ForwardVector * FVector::DotProduct(NewVelocity, ForwardVector);
					CollisionBoxPrimitive->SetPhysicsLinearVelocity(NewVelocity);
				}
			}
			if(ForwardSpeed < 0.0f)
			{
				FVector HandbrakeForce = ForwardVector * 1000000.0f * HandbrakeForceFactor;
				CollisionBoxPrimitive->AddForce(HandbrakeForce);

				FVector NewVelocity = CollisionBoxPrimitive->GetComponentVelocity();
				if(FVector::DotProduct(NewVelocity, ForwardVector) > 0.0f)
				{
					NewVelocity -= -ForwardVector * FVector::DotProduct(NewVelocity, ForwardVector);
					CollisionBoxPrimitive->SetPhysicsLinearVelocity(NewVelocity);
				}
			}
		}
		if(CurrentForwardSpeed > SpeedThreshold) bIsDrifting = true;
	}
}

void ABasePhysicsVehiclePawn::ApplyFrictionForces()
{
	if(!bIsInAir)
	{
		FVector WorldVelocity = CollisionBoxPrimitive->GetComponentVelocity();
	
		FVector LocalVelocity = CollisionBoxPrimitive->GetComponentTransform().InverseTransformVectorNoScale(WorldVelocity);
	
		float LocalSidewaysVelocity = LocalVelocity.Y;
		float LocalForwardVelocity = LocalVelocity.X;

		if (LocalSidewaysVelocity < 25.0f && LocalSidewaysVelocity > -25.0f && (LocalForwardVelocity < 5.0f && LocalForwardVelocity > -5.0f) && FMath::IsNearlyZero(ThrottleInput) && FMath::IsNearlyZero(BrakingInput))
		{
			CollisionBoxPrimitive->SetPhysicsLinearVelocity(FVector::ZeroVector);
			CollisionBoxPrimitive->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
			bLockUpVehicle = true;
			return;
		}
		bLockUpVehicle = false;
	
		FVector OppositeLocalSidewaysVelocity = FVector(0.0f, -LocalSidewaysVelocity, 0.0f);
		FVector OppositeLocalForwardVelocity = FVector(-LocalForwardVelocity, 0, 0.0f); 
	
		FVector OppositeWorldSidewaysVelocity = CollisionBoxPrimitive->GetComponentTransform().TransformVectorNoScale(OppositeLocalSidewaysVelocity);
		FVector OppositeWorldForwardVelocity = CollisionBoxPrimitive->GetComponentTransform().TransformVectorNoScale(OppositeLocalForwardVelocity);

		if(bIsDrifting)
		{
			FVector DriftDirection = CollisionBoxPrimitive->GetForwardVector().RotateAngleAxis(CalculateDriftAngle(), FVector(0,0,1));
			FVector DriftAssistForce = DriftDirection * DriftAssistFactor;
			DriftAssistForce *= 1.0f + (ThrottleInput * ThrottleDriftAssistanceBoost); //Amplify drift force by throttle
			
			CollisionBoxPrimitive->AddForce(DriftAssistForce, NAME_None, false);
			// Debug visualization
			if (bDebugForces)
			{
				DrawDebugDirectionalArrow(
					GetWorld(),
					CollisionBoxPrimitive->GetComponentLocation(),
					CollisionBoxPrimitive->GetComponentLocation() + DriftAssistForce * 0.1,
					100.0f, 
					FColor::Purple,
					false, 
					-1.0f, 
					0, 
					3.0f
				);
			}
		}

		float FrictionCoefficient = 0;
		float GripMultiplier = 0;
		for(const UPhysicalMaterial* Surface : CurrentSurfaceMaterials)
		{
			if(Surface == nullptr) return;
			FrictionCoefficient += (CurrentSpeed < 60.0f) ? Surface->StaticFriction : Surface->Friction;
			switch (Surface->SurfaceType)
			{
			case SurfaceType1: //Tarmac
				GripMultiplier += WheelConfig.TarmacGripMultiplier;
				break;
			case SurfaceType2: //Gravel
				GripMultiplier += WheelConfig.LightOffRoadGripMultiplier;
				break;
			case SurfaceType3: //Snow
				GripMultiplier += WheelConfig.LightOffRoadGripMultiplier;
				break;
			case SurfaceType4: //Mud
				GripMultiplier += WheelConfig.HeavyOffRoadGripMultiplier;
				break;
			default: //Shouldn't really ever be the case
				UE_LOG(LogTemp, Error, TEXT("No Correctly assigned Surface Material for friction effects"));
				GripMultiplier = 1.0f;
				break;
			}
		}
		FrictionCoefficient/=4;
		GripMultiplier/=4;
		
		
		if(IsGrounded() && (LocalSidewaysVelocity < -25.0f || LocalSidewaysVelocity > 25.0f))
		{
			float LateralFrictionMagnitude = FrictionCoefficient * CollisionBoxPrimitive->GetMass() * 9.81f * LateralFrictionFactor * GripMultiplier;
			if(bIsDrifting)
			{
				LateralFrictionMagnitude *= 0.5f;
				
				float DriftAngleFactor = FMath::Clamp(FMath::Abs(CalculateDriftAngle()) / MaxDriftAngle, 0.0f, 1.0f);
				float ThrottleFactor = FMath::Clamp(1.0f - FMath::Abs(ThrottleInput), 0.5f, 1.0f);
				LateralFrictionMagnitude *= (1.0f + DriftAngleFactor) * ThrottleFactor;
			}
			FVector LateralFrictionForce = OppositeWorldSidewaysVelocity.GetSafeNormal() * LateralFrictionMagnitude;
			CollisionBoxPrimitive->AddForce(LateralFrictionForce, NAME_None, false);
			if(bDebugForces)
			{
				DrawDebugDirectionalArrow(
				GetWorld(),
				CollisionBoxPrimitive->GetComponentLocation(),
				CollisionBoxPrimitive->GetComponentLocation() + LateralFrictionForce * 0.0001f, // scale down for visualization
				100.0f,   // arrow size
				FColor::Blue,   // color for lateral friction
				false,          // do not persist (lasts one frame)
				-1.0f,          // lifetime (not used here)
				0,              // depth priority
				3.0f            // thickness
			);
			}
		}

		if(IsGrounded() && !FMath::IsNearlyZero(LocalForwardVelocity))
		{
			float LongitudinalFrictionMagnitude = FrictionCoefficient * CollisionBoxPrimitive->GetMass() * 9.81f * LongitudinalFrictionFactor * FMath::Clamp(CurrentSpeed/MaxLongitudinalFrictionSpeed, 0, 1);
			FVector LongitudinalFrictionForce = OppositeWorldForwardVelocity.GetSafeNormal() * LongitudinalFrictionMagnitude; 
			CollisionBoxPrimitive->AddForce(LongitudinalFrictionForce, NAME_None, false);
			if(bDebugForces)
			{
				DrawDebugDirectionalArrow(
				GetWorld(),
				CollisionBoxPrimitive->GetComponentLocation(),
				CollisionBoxPrimitive->GetComponentLocation() + LongitudinalFrictionForce * 0.001f, // scale down for visualization
				100.0f,     // arrow size
				FColor::Red, // color for longitudinal friction
				false,       // do not persist (lasts one frame)
				-1.0f,       // lifetime (not used here)
				0,           // depth priority
				3.0f         // thickness
			);
			}
		}
	}
}

void ABasePhysicsVehiclePawn::ApplyDownforce()
{
	if(CollisionBoxPrimitive)
	{
		float Downforce = FMath::Pow(CurrentSpeed, 2) * DownforceFactor;
		FVector Force = FVector(0,0,-Downforce);

		if(bDebugForces)
		{
			FVector Start = CollisionBoxPrimitive->GetComponentLocation();
			FVector End = Start + (Force * 0.01f); // Scale for visibility

			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1f, 0, 2.0f); // Red downward line
		}

		CollisionBoxPrimitive->AddForce(Force, NAME_None, true);
	}
}

void ABasePhysicsVehiclePawn::UpdateEngine()
{
	if(CollisionBoxPrimitive)
	{
		const FVector ForwardVector = CollisionBoxPrimitive->GetForwardVector();
		const float ForwardVelocity = FVector::DotProduct(CollisionBoxPrimitive->GetComponentVelocity(), ForwardVector);
		if(ThrottleInput == 0 && IsGrounded())
		{
			//Engine braking?
			const float BrakingForceMagnitude = -ForwardVelocity * EngineBrakeForceFactor * 100;
			CollisionBoxPrimitive->AddForce(ForwardVector * BrakingForceMagnitude);
		}
		CurrentForwardSpeed = ForwardVelocity / 33.33;
		CurrentSpeed = CollisionBoxPrimitive->GetComponentVelocity().Size() / 33.33;
		UpdateGear();
		UpdateRPM();
		FString SpeedString = FString::Printf(TEXT("Current Speed: %d" " km/h"), FMath::RoundToInt(CurrentSpeed));
		FString RPMString = FString::Printf(TEXT("Current RPM: %d"), FMath::RoundToInt(CurrentRPM));
		FString GearString = FString::Printf(TEXT("Current Gear: %d"), GetCurrentGear());
		if(bDebugEngine)
		{
			DrawDebugString(GetWorld(), CollisionBoxPrimitive->GetComponentLocation() + FVector(0, -25, 50), SpeedString, nullptr, FColor::White, 0.0f, true);
			DrawDebugString(GetWorld(), CollisionBoxPrimitive->GetComponentLocation() + FVector(0, -25, 60), RPMString, nullptr, FColor::White, 0.0f, true);
			DrawDebugString(GetWorld(), CollisionBoxPrimitive->GetComponentLocation() + FVector(0, -25, 70), GearString, nullptr, FColor::White, 0.0f, true);
		}
		
	}
}

void ABasePhysicsVehiclePawn::UpdateRPM()
{
	float GearRatio = FMath::Abs(CurrentSpeed)/ GearboxConfig[CurrentGear - 1].MaxSpeed;
	CurrentRPM = (MaxRPM - IdleRPM) * GearRatio + IdleRPM;
	
	if(CurrentRPM > MaxRPM)
	{
		CurrentRPM = MaxRPM;
		bIsRedlining = true;
	}
	else bIsRedlining = false;
}

void ABasePhysicsVehiclePawn::UpdateGear()
{
	if(!bIsShifting)
	{
		if(CurrentSpeed > GearboxConfig[CurrentGear - 1].MaxSpeed && CurrentGear < GearboxConfig.Num() && CurrentForwardSpeed > 0)
		{
			bIsShifting = true;
			CurrentGear++;
			GetWorld()->GetTimerManager().SetTimer(ShiftTimerHandle, this, &ABasePhysicsVehiclePawn::ResetShifting, ShiftDelay, false);
		}
		else if(CurrentSpeed < GearboxConfig[CurrentGear - 1].MaxSpeed * DownshiftFactor && CurrentGear > 1)
		{
			bIsShifting = false;
			CurrentGear--;
			GetWorld()->GetTimerManager().SetTimer(ShiftTimerHandle, this, &ABasePhysicsVehiclePawn::ResetShifting, ShiftDelay, false);
		}
	}
	
}

void ABasePhysicsVehiclePawn::UpdateWheelAnimations(float DeltaTime)
{
	if(VehicleMeshComponent)
	{
		TArray<FString> WheelBoneNames = { "Wheel_Front_Left", "Wheel_Front_Right", "Wheel_Rear_Left", "Wheel_Rear_Right"};

		//Suspension animations
		if(IsGrounded())
		{
			int32 Counter = 0;
			for (auto BoneName : WheelBoneNames)
			{
				VehicleMeshComponent->SetBoneLocationByName(FName(*BoneName), WheelPositions[Counter] + FVector(0, 0, -SuspensionHeights[WheelBoneNames.Find(BoneName)] + WheelRadius), EBoneSpaces::WorldSpace);
				Counter++;
			}
		}
		else
		{
			int32 Counter = 0;
			for (auto BoneName : WheelBoneNames)
			{
				VehicleMeshComponent->SetBoneLocationByName(FName(*BoneName), WheelPositions[Counter] + FVector(0, 0, -MaxSuspensionExtent + WheelRadius), EBoneSpaces::WorldSpace);
				Counter++;
			}
		}

		//Steering Animations
		VehicleMeshComponent->SetBoneRotationByName("Wheel_Front_Left", VehicleMeshComponent->GetSocketRotation("Wheel_Front_Left_Locator") + FRotator(0, CurrentSteeringInput * SteeringSensitivityCurve->GetFloatValue(CurrentSpeed), 0), EBoneSpaces::WorldSpace);
		VehicleMeshComponent->SetBoneRotationByName("Wheel_Front_Right", VehicleMeshComponent->GetSocketRotation("Wheel_Front_Right_Locator") + FRotator(0, CurrentSteeringInput * SteeringSensitivityCurve->GetFloatValue(CurrentSpeed), 0), EBoneSpaces::WorldSpace);

		VehicleMeshComponent->SetBoneRotationByName("Wheel_Rear_Left", VehicleMeshComponent->GetSocketRotation("Wheel_Rear_Left_Locator") + FRotator(0, 0, 0), EBoneSpaces::WorldSpace);
		VehicleMeshComponent->SetBoneRotationByName("Wheel_Rear_Right", VehicleMeshComponent->GetSocketRotation("Wheel_Rear_Right_Locator") + FRotator(0, 0, 0), EBoneSpaces::WorldSpace);
		
		//Wheel Rotation Animations
		for (auto BoneName : WheelBoneNames)
		{
			FRotator BoneRotation = VehicleMeshComponent->GetBoneRotationByName(FName(*BoneName), EBoneSpaces::ComponentSpace);
			float RotationSpeed = CurrentSpeed * 6689 / (2 * PI * WheelRadius) * 360;
			float DeltaRotation = -RotationSpeed * DeltaTime;
			FRotator Rotation = BoneRotation;
			Rotation.Pitch += DeltaRotation;
			VehicleMeshComponent->SetBoneRotationByName(FName(*BoneName), Rotation, EBoneSpaces::ComponentSpace);
		}
		
	}
	
}

int ABasePhysicsVehiclePawn::GetCurrentGear()
{
	
	return CurrentSpeed > 0.0f || !FMath::IsNearlyZero(ThrottleInput) ? CurrentGear : -1;
}

void ABasePhysicsVehiclePawn::UpdateEngineSFX()
{
	//Magic numbers are minimum and maximum frequency for given sound.
	const float MappedEngineRotationSpeed = FMath::GetMappedRangeValueClamped(FVector2d(IdleRPM, MaxRPM), FVector2d(0.0f, 1.0f), CurrentRPM);
	EngineAudioComponent->SetFloatParameter(TEXT("EngineRPM"), MappedEngineRotationSpeed);
}

float ABasePhysicsVehiclePawn::CalculateDriftAngle() const
{
	FVector ForwardVector = CollisionBoxPrimitive->GetForwardVector();
	FVector Velocity = CollisionBoxPrimitive->GetPhysicsLinearVelocity();
	ForwardVector.Normalize();
	Velocity.Normalize();

	float DotProduct = FVector::DotProduct(ForwardVector, Velocity);
	float DriftAngle = FMath::Acos(FMath::Clamp(DotProduct, -1.0f, 1.0f)); //Drift Angle in Radians
	DriftAngle = FMath::RadiansToDegrees(DriftAngle);
	if(FVector::CrossProduct(ForwardVector, Velocity).Z < 0) DriftAngle = -DriftAngle;

	return DriftAngle;
}

void ABasePhysicsVehiclePawn::InitAudio()
{
	if(EngineAudioComponent)
	{
		EngineAudioComponent->SetActive(bPlaySound);
	}
}

//returns true if all wheels are gounded
bool ABasePhysicsVehiclePawn::IsGrounded()
{
	for (bool GroundPosition : WheelGroundPositions)
	{
		if(!GroundPosition) return false;
	}

	return true;
}

void ABasePhysicsVehiclePawn::IsInAir()
{
	for (bool GroundPosition : WheelGroundPositions)
	{
		if(GroundPosition)
		{
			bIsInAir = false;
			return;
		}
	}
	
	bIsInAir = true;
}

FVector2D ABasePhysicsVehiclePawn::GetCurrentTargetScreenPosition() const
{
	return CurrentTargetScreenPosition;
}

bool ABasePhysicsVehiclePawn::GetCurrentCameraLockSetting() const
{
	return bCameraLock;
}

void ABasePhysicsVehiclePawn::AttachPrimaryWeaponToVehicle(const TSubclassOf<ABaseWeapon>& WeaponClass)
{
	//Attach chosen weapon classes to player vehicle
	//Weapon ID must be the same as corresponding socket names
	
	if(WeaponClass)
	{
		UE_LOG(LogTemp, Log, TEXT("Attaching Primary Weapon"))
		if (!PrimaryWeapon)
		{
			PrimaryWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass);
			PrimaryWeapon->AttachToComponent(VehicleMeshComponent,FAttachmentTransformRules::KeepRelativeTransform, PrimaryWeaponID); //Connect to corresponding reference joint
			PrimaryWeapon->SetOwner(this);
			PrimaryWeapon->OwnerReference = this;
		}
		else if (PrimaryWeapon)
		{
			PrimaryWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			PrimaryWeapon->Destroy();
			
			PrimaryWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass);
			PrimaryWeapon->AttachToComponent(VehicleMeshComponent,FAttachmentTransformRules::KeepRelativeTransform, PrimaryWeaponID); //Connect to corresponding reference joint
			PrimaryWeapon->SetOwner(this);
			PrimaryWeapon->OwnerReference = this;
		}
		
		
	}
	if (Controller)
	{
		if (Controller->IsA<AMyPlayerController>()) //if the controller is a player
		{
			Cast<AMyPlayerController>(Controller)->UpdateWeaponIcons();
		}
	}
	
	
}

void ABasePhysicsVehiclePawn::AttachSecondaryWeaponToVehicle(const TSubclassOf<ABaseWeapon>& WeaponClass)
{
	
	if(WeaponClass)
	{
		UE_LOG(LogTemp, Log, TEXT("Attaching Secondary Weapon"))
		if (!SecondaryWeapon)
		{
			SecondaryWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass);
			SecondaryWeapon->AttachToComponent(VehicleMeshComponent,FAttachmentTransformRules::KeepRelativeTransform, SecondaryWeaponID); //Connect to corresponding reference joint
			SecondaryWeapon->SetOwner(this);
			SecondaryWeapon->OwnerReference = this;
		}
		else if (SecondaryWeapon)
		{
			SecondaryWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			SecondaryWeapon->Destroy();

			SecondaryWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass);
			SecondaryWeapon->AttachToComponent(VehicleMeshComponent,FAttachmentTransformRules::KeepRelativeTransform, SecondaryWeaponID); //Connect to corresponding reference joint
			SecondaryWeapon->SetOwner(this);
			SecondaryWeapon->OwnerReference = this;
		}
		
	}

	if (Controller)
	{
		if (Controller->IsA<AMyPlayerController>()) //if the controller is a player
		{
			Cast<AMyPlayerController>(Controller)->UpdateWeaponIcons();
		}
	}
	
}

TArray<FVector> ABasePhysicsVehiclePawn::GetWheelPositions()
{
	return WheelPositions;
}

void ABasePhysicsVehiclePawn::SetSteeringInput(float NewInput)
{
	CurrentSteeringInput = FMath::Clamp(NewInput, -1.0f, 1.0f);
}

void ABasePhysicsVehiclePawn::SetThrottleInput(float NewInput)
{
	ThrottleInput = FMath::Clamp(NewInput, -1.0f, 1.0f);
}

void ABasePhysicsVehiclePawn::SetBrakingInput(float NewInput)
{
	BrakingInput = FMath::Clamp(NewInput, -1.0f, 1.0f);
}

void ABasePhysicsVehiclePawn::SetHandbrakeInput(bool NewInput)
{
	bHandbrake = NewInput;
}

