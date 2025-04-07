// Fill out your copyright notice in the Description page of Project Settings.


#include "MachineGun.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/DecalComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/AudioComponent.h"
#include "NiagaraComponent.h"
#include "CarsNGuns/Vehicles/PlayerVehicleBase.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"

AMachineGun::AMachineGun()
{
	WeaponType = EWeaponType::MachineGun;
	UpgradeDamageType = EUpgradeDamageType::Bullet;
	
	TracerNiagaraComponentLeft = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NS_TracerSystemLeft"));
	TracerNiagaraComponentLeft->AttachToComponent(WeaponMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("BulletSpawnPoint_L"));

	TracerNiagaraComponentRight = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NS_TracerSystemRight"));
	TracerNiagaraComponentRight->AttachToComponent(WeaponMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("BulletSpawnPoint_R"));

	MuzzleFlashNiagaraComponentLeft = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NS_MuzzleFlashSystemLeft"));
	MuzzleFlashNiagaraComponentLeft->AttachToComponent(WeaponMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("BulletSpawnPoint_L"));
	
	MuzzleFlashNiagaraComponentRight = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NS_MuzzleFlashSystemRight"));
	MuzzleFlashNiagaraComponentRight->AttachToComponent(WeaponMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("BulletSpawnPoint_R"));

	//Create Audio Component(s)
	FiringAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FiringAudioComponent"));
}

void AMachineGun::BeginPlay()
{
	Super::BeginPlay();

	InitVFX();
	InitAudio();

	CurrentAmmo = MaxAmmo;
}

void AMachineGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveGunsTowardTarget(DeltaTime, 25.0f);
}

void AMachineGun::Fire()
{
	Super::Fire();
	
	if(bCanFire && CurrentAmmo > 0)
	{
		PerformHitScan();

		CurrentAmmo -= AmmoPerShot;
		if(!FiringAudioComponent->IsPlaying() || bIsWindingDown)
		{
			FiringAudioComponent->Activate(true);
			bIsWindingDown = false;
		}

		bCanFire = false;

		GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &AMachineGun::ResetFire, 1*FireRate, false);
	}
	else if(CurrentAmmo <= 0 && !bIsReloading)
	{
		bIsReloading = true;
		Reload();
	}
}

void AMachineGun::Reload()
{
	UE_LOG(LogTemp, Log, TEXT("Reloading..."));

	bCanFire = false;
	bIsReloading = true;
	StopFire();

	GetWorld()->GetTimerManager().ClearTimer(FireRateTimerHandle);

	// Set up values for tick-based reload
	ReloadProgress = 0.0f;
	ReloadedAmmoPerTick = MaxAmmo / (ReloadTime / ReloadTickInterval);
	CurrentAmmo = 0.0f; // Start from empty to refill gradually

	GetWorld()->GetTimerManager().SetTimer(
		ReloadTickHandle,
		this,
		&AMachineGun::ReloadTick,
		ReloadTickInterval,
		true
	);
}

void AMachineGun::FinishReload()
{
	CurrentAmmo = MaxAmmo;
	bIsReloading = false;
	
	UE_LOG(LogTemp, Log, TEXT("Reloaded!"));
	
	ResetFire();
}

void AMachineGun::ReloadTick()
{
	CurrentAmmo += ReloadedAmmoPerTick;
	ReloadProgress += ReloadTickInterval;

	if (CurrentAmmo >= MaxAmmo || ReloadProgress >= ReloadTime)
	{
		CurrentAmmo = MaxAmmo;
		bIsReloading = false;

		GetWorld()->GetTimerManager().ClearTimer(ReloadTickHandle);

		UE_LOG(LogTemp, Log, TEXT("Reloaded!"));
		ResetFire();
	}
}

void AMachineGun::StopFire()
{
	if(!bIsWindingDown)
	{
		if(FiringAudioComponent->IsPlaying())
		{
			FiringAudioComponent->SetTriggerParameter(TEXT("WindDown"));
			bIsWindingDown = true;
		}
	}
}

float AMachineGun::GetResourceBarValue() const
{
	return CurrentAmmo/MaxAmmo;
}


void AMachineGun::PerformHitScan()
{
	FVector ViewLocation;
	FRotator ViewRotation;

	if(PlayerController) //Player Controller Weapon
	{
		PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
	}
	else //AI Controlled weapon, probably
	{
		ViewRotation = GetOwner()->GetActorRotation();
	}
		
	//Collision Query Params
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); //Ignore Weapon
	Params.AddIgnoredActor(GetOwner()); //Ignore Weapon holder

	PerformGunTrace(WeaponMesh->GetSocketLocation(TEXT("BulletSpawnPoint_L")), WeaponMesh->GetSocketRotation(TEXT("BulletSpawnPoint_L")), HitResult, Params, ViewRotation.Vector());
	PerformGunTrace(WeaponMesh->GetSocketLocation(TEXT("BulletSpawnPoint_R")), WeaponMesh->GetSocketRotation(TEXT("BulletSpawnPoint_R")), HitResult, Params, ViewRotation.Vector());
			
	//Aktivera Tracer VFX
	TracerNiagaraComponentRight->Activate(true); 
	TracerNiagaraComponentLeft->Activate(true);

	//Aktivera MuzzleFlash VFX (behöver nog använda spawn rates på samma sätt som fire rate.)
	MuzzleFlashNiagaraComponentLeft->Activate(true);
	MuzzleFlashNiagaraComponentRight->Activate(true);
}

void AMachineGun::PerformGunTrace(const FVector& BarrelLocation, const FRotator& BarrelRotation, FHitResult HitResult, const FCollisionQueryParams& Params, const FVector& HitFromDirection)
{
	FVector AimDirection = BarrelRotation.Vector();

	float RandomRecoilAngleX = FMath::RandRange(-MaxRecoilAngle, MaxRecoilAngle);
	float RandomRecoilAngleY = FMath::RandRange(-MaxRecoilAngle, MaxRecoilAngle);

	RandomRecoilAngleX = FMath::DegreesToRadians(RandomRecoilAngleX);
	RandomRecoilAngleY = FMath::DegreesToRadians(RandomRecoilAngleY);

	FRotator RecoilRotation = FRotator(RandomRecoilAngleX, RandomRecoilAngleY, 0.0f);

	FVector RecoilAdjustedDirection = RecoilRotation.RotateVector(AimDirection);

	const FVector TraceEnd = BarrelLocation + RecoilAdjustedDirection*MaxRange;
			
	if(GetWorld()->LineTraceSingleByChannel(HitResult, BarrelLocation, TraceEnd, ECC_Visibility, Params))
	{
		/*
		if (HitResult.IsValidBlockingHit())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Valid: %s"), *HitResult.GetActor()->GetName());
		}
		*/
		AActor* HitActor = HitResult.GetActor();
		if(HitActor)
		{

			//Damage Dropoff
			float Distance = FVector::Dist(BarrelLocation, HitResult.ImpactPoint);
			float AppliedDamage = (1-Distance/10000)*Damage;
			
			UGameplayStatics::ApplyPointDamage(HitActor, AppliedDamage, HitFromDirection, HitResult, GetOwner()->GetInstigatorController(), this, DamageType);
		}
		if(UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAttached(BulletHoleDecalMaterial,
			FVector(5.0f,10.0f,10.0f),
			HitResult.GetComponent(),
			NAME_None,
			HitResult.ImpactPoint,
			HitResult.ImpactNormal.Rotation(),
			EAttachLocation::KeepWorldPosition,
			5.0f))
		{
			DecalComponent->SetFadeScreenSize(0);
			
			//Bullethole VFX
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletHoleDebrisNiagaraSystem, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
		}
		//DrawDebugLine(GetWorld(), BarrelLocation, HitResult.ImpactPoint, FColor::Green, false, 0.5f, 0, 1.0f);
	}
	else //If nothing is hit
	{
		UE_LOG(LogTemp, Warning, TEXT("Aint shooting shit"));
	}
	
}

void AMachineGun::MoveGunsTowardTarget(float DeltaTime, float InterpSpeed) const
{
    if (OwnerReference)
    {
        if (AController* Controller = OwnerReference->GetController())
        {
            if (Controller->IsA<APlayerController>()) // Player Controlled Weapon
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
                            FVector TraceEnd = WorldLocation + WorldDirection * MaxRange;

                            FHitResult HitResult;
                            FCollisionQueryParams Params;
                            Params.AddIgnoredActor(this); // Ignore Weapon
                            Params.AddIgnoredActor(GetOwner()); // Ignore Weapon holder

                            bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TraceEnd, ECC_Visibility, Params);

                            const FRotator CurrentRotationLeft = WeaponMesh->GetBoneRotationByName(TEXT("GunTargetingJoint_L"), EBoneSpaces::WorldSpace);
                            const FRotator CurrentRotationRight = WeaponMesh->GetBoneRotationByName(TEXT("GunTargetingJoint_R"), EBoneSpaces::WorldSpace);

                            if (bHit) // Shots are going to hit something
                            {
                                FRotator LookAtRotationLeft = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("BulletSpawnPoint_L")), HitResult.ImpactPoint);
                                FRotator LookAtRotationRight = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("BulletSpawnPoint_R")), HitResult.ImpactPoint);

                                LookAtRotationLeft = ClampTargetRotation(LookAtRotationLeft, GetOwner(), 45.0f, 10.0f);
                                LookAtRotationRight = ClampTargetRotation(LookAtRotationRight, GetOwner(), 45.0f, 10.0f);

                                FRotator TargetRotationLeft = FMath::RInterpTo(CurrentRotationLeft, LookAtRotationLeft, DeltaTime, InterpSpeed);
                                FRotator TargetRotationRight = FMath::RInterpTo(CurrentRotationRight, LookAtRotationRight, DeltaTime, InterpSpeed);

                                WeaponMesh->SetBoneRotationByName(TEXT("GunTargetingJoint_L"), TargetRotationLeft, EBoneSpaces::WorldSpace);
                                WeaponMesh->SetBoneRotationByName(TEXT("GunTargetingJoint_R"), TargetRotationRight, EBoneSpaces::WorldSpace);
                            }
                            else
                            {
                                FRotator LookAtRotationLeft = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("BulletSpawnPoint_L")), TraceEnd);
                                FRotator LookAtRotationRight = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("BulletSpawnPoint_R")), TraceEnd);
                                LookAtRotationLeft = ClampTargetRotation(LookAtRotationLeft, GetOwner(), 45.0f, 10.0f);
                                LookAtRotationRight = ClampTargetRotation(LookAtRotationRight, GetOwner(), 45.0f, 10.0f);

                                FRotator TargetRotationLeft = FMath::RInterpTo(CurrentRotationLeft, LookAtRotationLeft, DeltaTime, InterpSpeed);
                                FRotator TargetRotationRight = FMath::RInterpTo(CurrentRotationRight, LookAtRotationRight, DeltaTime, InterpSpeed);

                                WeaponMesh->SetBoneRotationByName(TEXT("GunTargetingJoint_L"), TargetRotationLeft, EBoneSpaces::WorldSpace);
                                WeaponMesh->SetBoneRotationByName(TEXT("GunTargetingJoint_R"), TargetRotationRight, EBoneSpaces::WorldSpace);
                            }
                        }
                    }
                }
                else if (OwnerReference->GetCurrentCameraLockSetting())
                {
                    FVector WorldLocation;
                    FVector WorldDirection;
                    
                    if (PlayerController)
                    {
                        if (PlayerController->DeprojectScreenPositionToWorld(OwnerReference->GetCurrentTargetScreenPosition().X, OwnerReference->GetCurrentTargetScreenPosition().Y, WorldLocation, WorldDirection))
                        {
                            FVector TraceEnd = WorldLocation + WorldDirection * MaxRange;

                            FHitResult HitResult;
                            FCollisionQueryParams Params;
                            Params.AddIgnoredActor(this); // Ignore Weapon
                            Params.AddIgnoredActor(GetOwner()); // Ignore Weapon holder

                            bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TraceEnd, ECC_Visibility, Params);

                            const FRotator CurrentRotationLeft = WeaponMesh->GetBoneRotationByName(TEXT("GunTargetingJoint_L"), EBoneSpaces::WorldSpace);
                            const FRotator CurrentRotationRight = WeaponMesh->GetBoneRotationByName(TEXT("GunTargetingJoint_R"), EBoneSpaces::WorldSpace);

                            if (bHit) // Shots are going to hit something
                            {
                                FRotator LookAtRotationLeft = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("BulletSpawnPoint_L")), HitResult.ImpactPoint);
                                FRotator LookAtRotationRight = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("BulletSpawnPoint_R")), HitResult.ImpactPoint);

                                LookAtRotationLeft = ClampTargetRotation(LookAtRotationLeft, GetOwner(), 45.0f, 10.0f);
                                LookAtRotationRight = ClampTargetRotation(LookAtRotationRight, GetOwner(), 45.0f, 10.0f);

                                FRotator TargetRotationLeft = FMath::RInterpTo(CurrentRotationLeft, LookAtRotationLeft, DeltaTime, InterpSpeed);
                                FRotator TargetRotationRight = FMath::RInterpTo(CurrentRotationRight, LookAtRotationRight, DeltaTime, InterpSpeed);

                                WeaponMesh->SetBoneRotationByName(TEXT("GunTargetingJoint_L"), TargetRotationLeft, EBoneSpaces::WorldSpace);
                                WeaponMesh->SetBoneRotationByName(TEXT("GunTargetingJoint_R"), TargetRotationRight, EBoneSpaces::WorldSpace);
                            }
                            else
                            {
                                FRotator LookAtRotationLeft = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("BulletSpawnPoint_L")), WorldLocation);
                                FRotator LookAtRotationRight = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("BulletSpawnPoint_R")), WorldLocation);
                                LookAtRotationLeft = ClampTargetRotation(LookAtRotationLeft, GetOwner(), 45.0f, 10.0f);
                                LookAtRotationRight = ClampTargetRotation(LookAtRotationRight, GetOwner(), 45.0f, 10.0f);

                                FRotator TargetRotationLeft = FMath::RInterpTo(CurrentRotationLeft, LookAtRotationLeft, DeltaTime, InterpSpeed);
                                FRotator TargetRotationRight = FMath::RInterpTo(CurrentRotationRight, LookAtRotationRight, DeltaTime, InterpSpeed);

                                WeaponMesh->SetBoneRotationByName(TEXT("GunTargetingJoint_L"), TargetRotationLeft, EBoneSpaces::WorldSpace);
                                WeaponMesh->SetBoneRotationByName(TEXT("GunTargetingJoint_R"), TargetRotationRight, EBoneSpaces::WorldSpace);
                            }
                        }
                    }
                }
            }
            else if (Controller->IsA<AAIController>()) // AI Controlled Weapon
            {
            	const FRotator CurrentRotationLeft = WeaponMesh->GetBoneRotationByName(TEXT("GunTargetingJoint_L"), EBoneSpaces::WorldSpace);
            	const FRotator CurrentRotationRight = WeaponMesh->GetBoneRotationByName(TEXT("GunTargetingJoint_R"), EBoneSpaces::WorldSpace);
            	ABasePhysicsVehiclePawn* PlayerVehiclePawn = Cast<ABasePhysicsVehiclePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
            	if (PlayerVehiclePawn && !PlayerVehiclePawn->bIsDead)
            	{
					FVector PlayerLocation = PlayerVehiclePawn->GetActorLocation();
            		
            		FRotator LookAtRotationLeft = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("BulletSpawnPoint_L")), PlayerLocation);
            		FRotator LookAtRotationRight = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("BulletSpawnPoint_R")), PlayerLocation);

            		LookAtRotationLeft = ClampTargetRotation(LookAtRotationLeft, GetOwner(), 45.0f, 10.0f);
            		LookAtRotationRight = ClampTargetRotation(LookAtRotationRight, GetOwner(), 45.0f, 10.0f);

            		FRotator TargetRotationLeft = FMath::RInterpTo(CurrentRotationLeft, LookAtRotationLeft, DeltaTime, InterpSpeed);
            		FRotator TargetRotationRight = FMath::RInterpTo(CurrentRotationRight, LookAtRotationRight, DeltaTime, InterpSpeed);

            		WeaponMesh->SetBoneRotationByName(TEXT("GunTargetingJoint_L"), TargetRotationLeft, EBoneSpaces::WorldSpace);
            		WeaponMesh->SetBoneRotationByName(TEXT("GunTargetingJoint_R"), TargetRotationRight, EBoneSpaces::WorldSpace);
            	}
            	
            }
        }
    }
}


//Initierar Niagara komponenter i beginplay
void AMachineGun::InitVFX() const
{
	if(TracerNiagaraComponentLeft)
	{
		TracerNiagaraComponentLeft->SetAsset(TracerNiagaraSystem);
		TracerNiagaraComponentLeft->Deactivate();
	}
	if(TracerNiagaraComponentRight)
	{
		TracerNiagaraComponentRight->SetAsset(TracerNiagaraSystem);
		TracerNiagaraComponentRight->Deactivate();
	}
	if(MuzzleFlashNiagaraComponentLeft)
	{
		MuzzleFlashNiagaraComponentLeft->SetAsset(MuzzleFlashNiagaraSystem);
		MuzzleFlashNiagaraComponentLeft->Deactivate();
	}
	if(MuzzleFlashNiagaraComponentRight)
	{
		MuzzleFlashNiagaraComponentRight->SetAsset(MuzzleFlashNiagaraSystem);
		MuzzleFlashNiagaraComponentRight->Deactivate();
	}
}

void AMachineGun::InitAudio() const
{
	if(FiringAudioComponent)
	{
		FiringAudioComponent->SetActive(true);
		FiringAudioComponent->Stop();
	}
}





