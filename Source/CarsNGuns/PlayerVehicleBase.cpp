// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerVehicleBase.h"

#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "BaseWeapon.h"
#include "DefaultGameInstance.h"
#include "MyPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Blueprint/UserWidget.h"

APlayerVehicleBase::APlayerVehicleBase()
{
	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRootComponent"));
	CameraRoot->SetupAttachment(RootComponent);
	CameraRoot->SetUsingAbsoluteRotation(true);
	CameraRoot->SetRelativeRotation(FRotator::ZeroRotator);
	
	// Set up the spring arm
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(CameraRoot);
	SpringArmComponent->SetRelativeRotation(FRotator(DefaultCameraPitch, 0.f, 0.f)); 
	SpringArmComponent->TargetArmLength = SpringArmLength;      
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bUsePawnControlRotation = false;
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bDoCollisionTest = false;

	TargetZoomLevel = SpringArmComponent->TargetArmLength;

	//Create Camera Component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;
	
	MaxCameraDistance = 500.0f;

	DefaultYawOffset = SpringArmComponent->GetRelativeRotation().Yaw;

	//Init Health Component
	if(HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &APlayerVehicleBase::OnDeath);
		HealthComponent->ActiveArmorLevel = ArmorLevel;
	}

	
}

void APlayerVehicleBase::BeginPlay()
{
	Super::BeginPlay();

	//Set Playercontroller reference
	PlayerController = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	ParameterCollection = LoadObject<UMaterialParameterCollection>(nullptr, TEXT("/Game/Materials/OcclusionMask/MPC_Occlusion"));
	ParameterCollectionInstance = GetWorld()->GetParameterCollectionInstance(ParameterCollection);

	GetWorld()->GetTimerManager().SetTimer(UpdateTargetTimerHandle, this, &APlayerVehicleBase::UpdateTarget, 0.1, true);

	PlayerController->bShowMouseCursor = true;
	PlayerController->bEnableClickEvents = true;
	PlayerController->bEnableMouseOverEvents = true;

	if (bDebugAI)
	{
		if (PiPWidgetClass)
		{
			UUserWidget* PiPWidget = CreateWidget<UUserWidget>(GetWorld(), PiPWidgetClass);
			if (PiPWidget) PiPWidget->AddToViewport();
		}
	}
}

// Called to bind functionality to input
void APlayerVehicleBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Vehicle Control Axis
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &APlayerVehicleBase::ApplyThrottleInput);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &APlayerVehicleBase::ApplyThrottleInput);
		EnhancedInputComponent->BindAction(BrakingAction, ETriggerEvent::Triggered, this, &APlayerVehicleBase::ApplyBrakingInput);
		EnhancedInputComponent->BindAction(BrakingAction, ETriggerEvent::Completed, this, &APlayerVehicleBase::ApplyBrakingInput);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &APlayerVehicleBase::ApplySteeringInput);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &APlayerVehicleBase::ApplySteeringInput);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Triggered, this, &APlayerVehicleBase::ApplyHandbrakeInput);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &APlayerVehicleBase::ApplyHandbrakeInput);

		EnhancedInputComponent->BindAction(CameraToggleAction, ETriggerEvent::Completed, this, &APlayerVehicleBase::CameraModeToggle);
		EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &APlayerVehicleBase::HandleZoom);

		//Gun Control
		EnhancedInputComponent->BindAction(FirePrimaryAction, ETriggerEvent::Triggered, this, &APlayerVehicleBase::FirePrimary);
		EnhancedInputComponent->BindAction(FirePrimaryAction, ETriggerEvent::Completed, this, &APlayerVehicleBase::FirePrimary);
		EnhancedInputComponent->BindAction(FireSecondaryAction, ETriggerEvent::Triggered, this, &APlayerVehicleBase::FireSecondary);
		EnhancedInputComponent->BindAction(FireSecondaryAction, ETriggerEvent::Completed, this, &APlayerVehicleBase::FireSecondary);

		//Debug Actions
		EnhancedInputComponent->BindAction(CarFlipAction, ETriggerEvent::Completed, this, &APlayerVehicleBase::FlipCar);
		EnhancedInputComponent->BindAction(DebugMenuAction, ETriggerEvent::Started, this, &APlayerVehicleBase::ToggleDebugMenu);
	}

}

void APlayerVehicleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if(CurrentTarget) UE_LOG(LogTemp, Log, TEXT("Nearest Target: %s"), *CurrentTarget->GetName())
	//else UE_LOG(LogTemp, Log, TEXT("No Target in View"))

	//UE_LOG(LogTemp, Log, TEXT("Camera Lock Setting: %d"), bCameraLock)

	//Zooms camera if requested by targetzoomlevel
	if(SpringArmComponent->TargetArmLength != TargetZoomLevel)
	{
		SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoomLevel, DeltaTime, CameraZoomSmoothingSpeed);
		UpdateCameraPitch();
	}
	UpdateCameraYaw(DeltaTime);

	UpdateCameraMode();
	
	UpdateCurrentTargetScreenPosition();
	UpdatePlayerScreenPosition();

	if(!CurrentTarget) bCameraLock = false;

	 //IF Camera Lock is False (Free cam)
	if(!bCameraLock && CurrentCameraMode == ECameraMode::Isometric)
	{
		float MouseX;
		float MouseY;
		PlayerController->GetMousePosition(MouseX, MouseY);

		FVector WorldLocation;
		FVector WorldDirection;
		
		if(PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection))
		{
			FVector TraceStart = WorldLocation;
			FVector TraceEnd = WorldLocation + WorldDirection * 10000.0f;

			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			FVector TargetLocation = GetActorLocation();
			if(GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Camera, QueryParams))
			{
				TargetLocation.X = HitResult.Location.X;
				TargetLocation.Y = HitResult.Location.Y;
			}

			FVector PlayerLocation = GetActorLocation();
			FVector Offset = TargetLocation - PlayerLocation;

			if(Offset.Size() > MaxCameraDistance) Offset = Offset.GetSafeNormal() * MaxCameraDistance;

			FVector CameraFocus = PlayerLocation + Offset;
			FVector SmoothedLocation = FMath::VInterpTo(SpringArmComponent->GetComponentLocation(), CameraFocus, DeltaTime, CameraSensitivity);
			SpringArmComponent->SetWorldLocation(SmoothedLocation);
			SpringArmComponent->CameraRotationLagSpeed = 5.0f;
		}
	}
	
	else if(CurrentCameraMode == ECameraMode::ThirdPerson)
	{
		FVector SmoothedLocation = FMath::VInterpTo(SpringArmComponent->GetComponentLocation(), GetActorLocation(), DeltaTime, CameraSensitivity);
		SpringArmComponent->SetWorldLocation(SmoothedLocation);
		SpringArmComponent->CameraRotationLagSpeed = 10.0f;
	}
	
	//IF Camera Lock is True (Target Lock Cam)
	else if(bCameraLock)
	{
		FVector PlayerLocation = GetActorLocation();
		FVector Offset = CurrentTarget->GetActorLocation() - PlayerLocation;

		if(Offset.Size() > MaxCameraDistance) Offset = Offset.GetSafeNormal() * MaxCameraDistance;

		FVector CameraFocus = PlayerLocation + Offset;
		FVector SmoothedLocation = FMath::VInterpTo(SpringArmComponent->GetComponentLocation(), CameraFocus, DeltaTime, CameraSensitivity);
		SpringArmComponent->SetWorldLocation(SmoothedLocation);
		SpringArmComponent->CameraRotationLagSpeed = 5.0f;
	}
}

void APlayerVehicleBase::ApplyThrottleInput(const FInputActionValue& Value)
{
	SetThrottleInput(Value.Get<float>());
}

void APlayerVehicleBase::ApplyBrakingInput(const FInputActionValue& Value)
{
	SetBrakingInput(Value.Get<float>());
}

void APlayerVehicleBase::ApplySteeringInput(const FInputActionValue& Value)
{
	SetSteeringInput(Value.Get<float>());
}

void APlayerVehicleBase::ApplyHandbrakeInput(const FInputActionValue& Value)
{
	SetHandbrakeInput(Value.Get<bool>());
}

void APlayerVehicleBase::HandleZoom(const FInputActionValue& Value)
{
	TargetZoomLevel = FMath::Clamp(SpringArmComponent->TargetArmLength - Value.Get<float>() * (50 * CameraZoomSensitivity), MinCameraZoom, MaxCameraZoom);
}

void APlayerVehicleBase::FirePrimary(const FInputActionValue& Value)
{
	if(PrimaryWeapon)
	{
		if(Value.Get<bool>())
		{
			PrimaryWeapon->Fire();
		}
		else if(!Value.Get<bool>())
		{
			PrimaryWeapon->StopFire();
		}
	}
}

void APlayerVehicleBase::FireSecondary(const FInputActionValue& Value)
{
	if(SecondaryWeapon)
	{
		if(Value.Get<bool>())
		{
			SecondaryWeapon->Fire();
		}
		else if(!Value.Get<bool>())
		{
			SecondaryWeapon->StopFire();
		}
	}
}


void APlayerVehicleBase::CameraModeToggle()
{
	if(CurrentTarget) bCameraLock ^= true;
	
}

bool APlayerVehicleBase::IsInView(FVector TargetLocation)
{
	FVector2D ScreenPosition;

	if(PlayerController)
	{
		PlayerController->ProjectWorldLocationToScreen(TargetLocation, ScreenPosition);
		FVector2D ScreenSize;
		int32 ViewportWidth, ViewportHeight;

		PlayerController->GetViewportSize(ViewportWidth, ViewportHeight);

		// Assign the int32 values to the FVector2D
		ScreenSize.X = static_cast<float>(ViewportWidth);
		ScreenSize.Y = static_cast<float>(ViewportHeight);

		if (ScreenPosition.X >= 0 && ScreenPosition.X <= ScreenSize.X && ScreenPosition.Y >= 0 && ScreenPosition.Y <= ScreenSize.Y)
		{
			return true;
		}
	}
	return false;
}

void APlayerVehicleBase::UpdateTarget()
{
	if(PlayerController)
	{
		UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if(GameInstance)
		{
			if(GameInstance->GetEnemyManager()->GetEnemies().Num() == 0)
			{
				CurrentTarget = nullptr;
				return;
			}

			FVector2D MousePosition;
			if(PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
			{
				AActor* NearestTarget = nullptr;
				float MinScreenDistanceSquared = FLT_MAX;

				for(AActor* Enemy : GameInstance->GetEnemyManager()->GetEnemies())
				{
					if(!Enemy || !IsInView(Enemy->GetActorLocation())) continue;
					FVector2D EnemyScreenPosition;
					if(PlayerController->ProjectWorldLocationToScreen(Enemy->GetActorLocation(), EnemyScreenPosition))
					{
						float ScreenDistanceSquared = FVector2D::DistSquared(MousePosition, EnemyScreenPosition);
						
						if(ScreenDistanceSquared < MinScreenDistanceSquared)
						{
							MinScreenDistanceSquared = ScreenDistanceSquared;
							NearestTarget = Enemy;
						}
					}
					
				}
				CurrentTarget = NearestTarget;
			}
			
			
		}
	}
}

void APlayerVehicleBase::UpdateCurrentTargetScreenPosition()
{
	if(!CurrentTarget)
	{
		//IF NO TARGET DONT DRAW TARGET
		bTargetVisible = false;
		return;
	}
	if(PlayerController)
	{
		FVector WorldLocation = CurrentTarget->GetActorLocation();
		FVector2D ScreenPosition;
		PlayerController->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition);
		CurrentTargetScreenPosition = ScreenPosition;
		bTargetVisible = true;
	}
}

void APlayerVehicleBase::UpdatePlayerScreenPosition()
{
	if(PlayerController)
	{
		if(ParameterCollectionInstance)
		{
			ParameterCollectionInstance->SetVectorParameterValue(FName("PlayerPosition"), GetActorLocation());
		}
	}
}

void APlayerVehicleBase::UpdateCameraPitch()
{
	if(SpringArmComponent->TargetArmLength >= ZoomRotationalThreshold)
	{
		//UE_LOG(LogTemp, Log, TEXT("Dont Rotate Camera"))
		SpringArmComponent->SetRelativeRotation(FRotator(DefaultCameraPitch, SpringArmComponent->GetRelativeRotation().Yaw, SpringArmComponent->GetRelativeRotation().Roll));
	}
	else
	{
		//UE_LOG(LogTemp, Log, TEXT("Trying to rotate Camera"))
		float NewPitch = FMath::GetMappedRangeValueClamped(FVector2D(MinCameraZoom, ZoomRotationalThreshold), FVector2d(MinPitch, DefaultCameraPitch), SpringArmComponent->TargetArmLength);
		SpringArmComponent->SetRelativeRotation(FRotator(NewPitch, SpringArmComponent->GetRelativeRotation().Yaw, SpringArmComponent->GetRelativeRotation().Roll));
	}
}

void APlayerVehicleBase::UpdateCameraYaw(float DeltaTime)
{
	FVector ForwardDirection = CollisionBoxPrimitive->GetForwardVector();

	if(!ForwardDirection.IsNearlyZero())
	{
		FRotator PlayerDirectionRotation = ForwardDirection.Rotation();
		DefaultYawOffset = PlayerDirectionRotation.Yaw;
		float ZoomFactor = FMath::Clamp(FMath::GetMappedRangeValueClamped(FVector2d(YawTransitionThreshold, MinCameraZoom), FVector2d(1.0f, 0.0f), SpringArmComponent->TargetArmLength), 0.0f, 1.0f);
		float DesiredYaw = FMath::Lerp(PlayerDirectionRotation.Yaw, DefaultYawOffset, ZoomFactor);
		FRotator CurrentRotation = SpringArmComponent->GetRelativeRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, FRotator(CurrentRotation.Pitch, DesiredYaw, 0.0f), DeltaTime, CameraZoomSmoothingSpeed);
		SpringArmComponent->SetRelativeRotation(NewRotation);
	}
}

void APlayerVehicleBase::UpdateCameraMode()
{
	if(SpringArmComponent->TargetArmLength > YawTransitionThreshold) CurrentCameraMode = ECameraMode::Isometric;
	
	else CurrentCameraMode = ECameraMode::ThirdPerson;
}

void APlayerVehicleBase::FlipCar()
{
	const float BaseImpulse = 750.0f;
	const float RandomRange = 200.0f;
	const float MaxAngularImpulse = 500.0f;
	FVector RandomImpulse = FVector(FMath::RandRange(-RandomRange, RandomRange), FMath::RandRange(-RandomRange, RandomRange), BaseImpulse + FMath::RandRange(0.0f, RandomRange));
	FVector RandomAngularImpulse = FVector(FMath::RandRange(-MaxAngularImpulse, MaxAngularImpulse), FMath::RandRange(-MaxAngularImpulse, MaxAngularImpulse), FMath::RandRange(-MaxAngularImpulse, MaxAngularImpulse));
	CollisionBoxPrimitive->AddImpulse(RandomImpulse, NAME_None, true);
	CollisionBoxPrimitive->AddAngularImpulseInDegrees(RandomAngularImpulse, NAME_None, true);
}

void APlayerVehicleBase::OnDeath()
{
	UE_LOG(LogTemp, Error, TEXT("Player just fucking died"))
	if(CollisionBoxPrimitive && !bIsDead)
	{
		const float BaseImpulse = 750.0f;
		const float RandomRange = 200.0f;
		const float MaxAngularImpulse = 500.0f;
		FVector RandomImpulse = FVector(FMath::RandRange(-RandomRange, RandomRange), FMath::RandRange(-RandomRange, RandomRange), BaseImpulse + FMath::RandRange(0.0f, RandomRange));
		FVector RandomAngularImpulse = FVector(FMath::RandRange(-MaxAngularImpulse, MaxAngularImpulse), FMath::RandRange(-MaxAngularImpulse, MaxAngularImpulse), FMath::RandRange(-MaxAngularImpulse, MaxAngularImpulse));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticleSystem, GetActorLocation(), GetActorRotation(), FVector(DeathExplosionScaleFactor));
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSoundCue, GetActorLocation(), GetActorRotation());
		CollisionBoxPrimitive->AddImpulse(RandomImpulse, NAME_None, true);
		CollisionBoxPrimitive->AddAngularImpulseInDegrees(RandomAngularImpulse, NAME_None, true);
		bIsDead = true;
		PlayerController->HandlePlayerDeath();
		GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &APlayerVehicleBase::DestroyActor, 3.0f, false); //Kill after time
	}
}

void APlayerVehicleBase::DestroyActor()
{
	if (PrimaryWeapon) PrimaryWeapon->Destroy();
	if (SecondaryWeapon) SecondaryWeapon->Destroy();
	

	//Somehow disable player dependent widgets;
	
	Destroy();
}

void APlayerVehicleBase::ToggleDebugMenu()
{
	if (Controller)
	{
		Cast<AMyPlayerController>(Controller)->ToggleDebugMenu();
	}
}

