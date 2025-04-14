// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserRifle.h"

#include "AIController.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "CarsNGuns/Vehicles/PlayerVehicleBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/PoseableMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ALaserRifle::ALaserRifle()
{
	WeaponType = EWeaponType::LaserRifle;
	DamageType = EDamageType::Special;
	//Create Audio Component(s)
	FiringAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FiringAudioComponent"));

	LaserBeamNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserBeamNiagaraComponent"));
	LaserBeamNiagaraComponent->AttachToComponent(WeaponMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("ProjectileSpawnPoint"));
}

void ALaserRifle::BeginPlay()
{
	Super::BeginPlay();

	InitVFX();
	InitAudio();
}

void ALaserRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTowardTarget(DeltaTime, 25.0f);

	//UE_LOG(LogTemp, Warning, TEXT("Current Heat Level: %f"), CurrentHeat);
}

void ALaserRifle::Fire()
{
	Super::Fire();

	if(bCanFire && CurrentHeat < MaxHeat && !bIsOverheated)
	{
		PerformHitScan();
		PerformBeamScan();

		GetWorld()->GetTimerManager().ClearTimer(PassiveCooldownTimerHandle);

		CurrentHeat += HeatPerTick;
		CurrentHeat = FMath::Clamp(CurrentHeat, 0.0f, MaxHeat);

		if(!FiringAudioComponent->IsPlaying() || bIsWindingDown)
		{
			FiringAudioComponent->Activate(true);
			bIsWindingDown = false;
		}

		if(CurrentHeat >= MaxHeat)
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentHeat = Overheat"));
			bIsOverheated = true;
			bCanFire = false;
			StopFire();
			GetWorld()->GetTimerManager().ClearTimer(PassiveCooldownTimerHandle); // Stop passive cooling
			GetWorld()->GetTimerManager().ClearTimer(CooldownDelayTimerHandle); //Stop Cooldown Delay
			GetWorld()->GetTimerManager().SetTimer(OverheatTimerHandle, this, &ALaserRifle::CoolDown, CooldownTime, false);
		}
		else
		{
			// Reset the cooldown delay timer (cooling starts only after 0.5s of not firing)
			GetWorld()->GetTimerManager().ClearTimer(CooldownDelayTimerHandle);
			GetWorld()->GetTimerManager().SetTimer(CooldownDelayTimerHandle, this, &ALaserRifle::StartPassiveCooldown, PassiveCooldownDelayTime, false);
		}

		bCanFire = false;

		GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &ALaserRifle::ResetFire, BaseFireRate / *OwnerReference->GetUpgradeHandlerComponent()->StatConfig.FireRateMultipliers.Find(DamageType), false);
	}
	else if (CurrentHeat < MaxHeat && !bIsOverheated)
	{
		PerformBeamScan();
	}
}

void ALaserRifle::StopFire()
{
	Super::StopFire();

	if(FiringAudioComponent->IsPlaying())
	{
		FiringAudioComponent->SetTriggerParameter(TEXT("WindDown"));
		bIsWindingDown = true;
	}
	LaserBeamNiagaraComponent->Deactivate();
}

float ALaserRifle::GetResourceBarValue() const
{
	return CurrentHeat/MaxHeat;
}

void ALaserRifle::CoolDown()
{
	bIsCoolingDown = true;
	bCanFire = false;

	// Start rapidly draining heat over OverheatCooldownDuration
	GetWorld()->GetTimerManager().SetTimer(
		OverheatCooldownTickHandle, 
		this, 
		&ALaserRifle::RapidCooldownTick, 
		PassiveHeatCooldownTickRate, 
		true
	);
}


void ALaserRifle::StartPassiveCooldown()
{
	CooldownTick();
	GetWorld()->GetTimerManager().SetTimer(PassiveCooldownTimerHandle, this, &ALaserRifle::CooldownTick, PassiveHeatCooldownTickRate, true);
}

void ALaserRifle::CooldownTick()
{
	if (CurrentHeat > 0)
	{
		float DrainPerTick = MaxHeat / (BaseReloadTime / *OwnerReference->GetUpgradeHandlerComponent()->StatConfig.RechargeMultipliers.Find(DamageType) / PassiveHeatCooldownTickRate);
		CurrentHeat -= DrainPerTick;
		CurrentHeat = FMath::Clamp(CurrentHeat, 0.0f, MaxHeat);
	}

	if (CurrentHeat <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(PassiveCooldownTimerHandle); // Stop cooling when fully cooled
	}
}

void ALaserRifle::RapidCooldownTick()
{
	float DrainPerTick = MaxHeat / (BaseOverheatCooldownDuration / *OwnerReference->GetUpgradeHandlerComponent()->StatConfig.RechargeMultipliers.Find(DamageType) / PassiveHeatCooldownTickRate);

	CurrentHeat -= DrainPerTick;
	CurrentHeat = FMath::Clamp(CurrentHeat, 0.0f, MaxHeat);

	if (CurrentHeat <= 0.0f)
	{
		CurrentHeat = 0.0f;
		bIsCoolingDown = false;
		bIsOverheated = false;

		LaserBeamNiagaraComponent->Deactivate();

		GetWorld()->GetTimerManager().ClearTimer(OverheatCooldownTickHandle);
		GetWorld()->GetTimerManager().SetTimer(FireResetTimerHandle, this, &ALaserRifle::ResetFire, 0.1, false);
	}
}


void ALaserRifle::PerformHitScan()
{
	FVector ViewLocation;
	FRotator ViewRotation;
	if(PlayerController)
	{
		PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
	}
	else
	{
		ViewRotation = GetOwner()->GetActorRotation();
	}
	//Collision Query Params
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	PerformGunTrace(WeaponMesh->GetSocketLocation(TEXT("ProjectileSpawnPoint")), WeaponMesh->GetSocketRotation(TEXT("ProjectileSpawnPoint")), HitResult, Params, ViewRotation.Vector());
}

void ALaserRifle::PerformBeamScan()
{
	FVector ViewLocation;
	FRotator ViewRotation;
	if(PlayerController)
	{
		PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
	}
	else
	{
		ViewRotation = GetOwner()->GetActorRotation();
	}
	//Collision Query Params
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	PerformBeamTrace(WeaponMesh->GetSocketLocation(TEXT("ProjectileSpawnPoint")), WeaponMesh->GetSocketRotation(TEXT("ProjectileSpawnPoint")), HitResult, Params, ViewRotation.Vector());
}

void ALaserRifle::PerformGunTrace(const FVector& BarrelLocation, const FRotator& BarrelRotation, FHitResult HitResult, const FCollisionQueryParams& Params, const FVector& HitFromDirection)
{
	FVector AimDirection = BarrelRotation.Vector();
	
	const FVector TraceEnd = BarrelLocation + AimDirection*MaxRange;

	if(GetWorld()->LineTraceSingleByChannel(HitResult, BarrelLocation, TraceEnd, ECC_Visibility, Params))
	{
		AActor* HitActor = HitResult.GetActor();
		if(HitActor)
		{
			float Distance = FVector::Dist(BarrelLocation, HitResult.ImpactPoint);
			float AppliedDamage = (1-Distance/10000)* BaseDamage * *OwnerReference->GetUpgradeHandlerComponent()->StatConfig.DamageMultipliers.Find(DamageType); //Damage Drop Off things
			UGameplayStatics::ApplyPointDamage(HitActor, AppliedDamage, HitFromDirection, HitResult, GetOwner()->GetInstigatorController(), this, DamageTypeClass);
		}
		//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Red, false, 1.0f);
	}
	else
	{
		//Hitting nothing
	}
}

void ALaserRifle::PerformBeamTrace(const FVector& BarrelLocation, const FRotator& BarrelRotation, FHitResult HitResult,
	const FCollisionQueryParams& Params, const FVector& HitFromDirection)
{
	FVector AimDirection = BarrelRotation.Vector();
	
	const FVector TraceEnd = BarrelLocation + AimDirection*MaxRange;

	if(GetWorld()->LineTraceSingleByChannel(HitResult, BarrelLocation, TraceEnd, ECC_Visibility, Params))
	{
		LaserBeamNiagaraComponent->SetVariablePosition(TEXT("LaserEnd"), HitResult.ImpactPoint);
		LaserBeamNiagaraComponent->Activate();

		
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LaserBeamImpactNiagaraSystem, HitResult.Location);
	}
	else
	{
		LaserBeamNiagaraComponent->SetVariablePosition(TEXT("LaserEnd"), TraceEnd);
		LaserBeamNiagaraComponent->Activate();
	}
	
}

void ALaserRifle::MoveTowardTarget(float DeltaTime, float InterpSpeed)
{
    if (OwnerReference)
    {
        if (AController* Controller = OwnerReference->GetController())
        {
            if (Controller->IsA<APlayerController>())
            {
                if (!OwnerReference->GetCurrentCameraLockSetting())
                {
                    if (PlayerController)
                    {
                        FVector2D MousePosition;
                        PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
                        FVector WorldLocation;
                        FVector WorldDirection;

                        if (PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection))
                        {
                            FVector TraceEnd = WorldLocation + WorldDirection * 10000.0f;

                            FHitResult HitResult;
                            FCollisionQueryParams Params;
                            Params.AddIgnoredActor(this); // Ignore Weapon
                            Params.AddIgnoredActor(GetOwner()); // Ignore Weapon holder

                            bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TraceEnd, ECC_Visibility, Params);

                            FRotator CurrentRotation = WeaponMesh->GetBoneRotationByName(TEXT("GunRotator"), EBoneSpaces::WorldSpace);

                            if (bHit)
                            {
                                FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
                                    WeaponMesh->GetSocketLocation(TEXT("ProjectileSpawnPoint")), HitResult.ImpactPoint);
                                LookAtRotation = ClampTargetRotation(LookAtRotation, GetOwner(), 360.0f, 20.0f);
                                FRotator TargetRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, InterpSpeed);
                                WeaponMesh->SetBoneRotationByName(TEXT("GunRotator"), TargetRotation, EBoneSpaces::WorldSpace);
                            }
                            else
                            {
                                FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
                                    WeaponMesh->GetSocketLocation(TEXT("ProjectileSpawnPoint")), TraceEnd);
                                LookAtRotation = ClampTargetRotation(LookAtRotation, GetOwner(), 360.0f, 20.0f);
                                FRotator TargetRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, InterpSpeed);
                                WeaponMesh->SetBoneRotationByName(TEXT("GunRotator"), TargetRotation, EBoneSpaces::WorldSpace);
                            }
                        }
                    }
                }
                else if (OwnerReference->GetCurrentCameraLockSetting())
                {
                    if (PlayerController)
                    {
                        FVector WorldLocation;
                        FVector WorldDirection;

                        if (PlayerController->DeprojectScreenPositionToWorld(
                                OwnerReference->GetCurrentTargetScreenPosition().X, 
                                OwnerReference->GetCurrentTargetScreenPosition().Y, 
                                WorldLocation, 
                                WorldDirection))
                        {
                            FVector TraceEnd = WorldLocation + WorldDirection * 10000.0f;

                            FHitResult HitResult;
                            FCollisionQueryParams Params;
                            Params.AddIgnoredActor(this); // Ignore Weapon
                            Params.AddIgnoredActor(GetOwner()); // Ignore Weapon holder

                            bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TraceEnd, ECC_Visibility, Params);

                            FRotator CurrentRotation = WeaponMesh->GetBoneRotationByName(TEXT("GunRotator"), EBoneSpaces::WorldSpace);

                            if (bHit)
                            {
                                FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
                                    WeaponMesh->GetSocketLocation(TEXT("ProjectileSpawnPoint")), HitResult.ImpactPoint);
                                LookAtRotation = ClampTargetRotation(LookAtRotation, GetOwner(), 360.0f, 20.0f);
                                FRotator TargetRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, InterpSpeed);
                                WeaponMesh->SetBoneRotationByName(TEXT("GunRotator"), TargetRotation, EBoneSpaces::WorldSpace);
                            }
                            else
                            {
                                FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
                                    WeaponMesh->GetSocketLocation(TEXT("ProjectileSpawnPoint")), WorldLocation);
                                LookAtRotation = ClampTargetRotation(LookAtRotation, GetOwner(), 360.0f, 20.0f);
                                FRotator TargetRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, InterpSpeed);
                                WeaponMesh->SetBoneRotationByName(TEXT("GunRotator"), TargetRotation, EBoneSpaces::WorldSpace);
                            }
                        }
                    }
                }
            }
        	else if (Controller->IsA<AAIController>())
        	{
        		FRotator CurrentRotation = WeaponMesh->GetBoneRotationByName(TEXT("GunRotator"), EBoneSpaces::WorldSpace);
        		ABasePhysicsVehiclePawn* PlayerVehiclePawn = Cast<ABasePhysicsVehiclePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
        		if (PlayerVehiclePawn && !PlayerVehiclePawn->bIsDead)
        		{
        			FVector PlayerLocation = PlayerVehiclePawn->GetActorLocation();
        			
        			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("ProjectileSpawnPoint")), PlayerLocation);
        			
        			LookAtRotation = ClampTargetRotation(LookAtRotation, GetOwner(), 360.0f, 20.0f);
        			
        			FRotator TargetRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, InterpSpeed);
        			WeaponMesh->SetBoneRotationByName(TEXT("GunRotator"), TargetRotation, EBoneSpaces::WorldSpace);
        		}
        	}
        }
    }
}


void ALaserRifle::InitVFX() const
{
	if(LaserBeamNiagaraComponent)
	{
		LaserBeamNiagaraComponent->SetAsset(LaserBeamNiagaraSystem);
		LaserBeamNiagaraComponent->Deactivate();
	}
}

void ALaserRifle::InitAudio() const
{
	if(FiringAudioComponent)
	{
		FiringAudioComponent->SetActive(true);
		FiringAudioComponent->Stop();
	}
}
