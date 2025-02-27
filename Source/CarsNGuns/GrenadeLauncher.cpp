// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeLauncher.h"
#include "BaseProjectile.h"
#include "GrenadeProjectile.h"
#include "PlayerVehicleBase.h"
#include "Components/AudioComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AGrenadeLauncher::AGrenadeLauncher()
{
	//Create Audio Component(s)
	FiringAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FiringAudioComponent"));
}

void AGrenadeLauncher::BeginPlay()
{
	Super::BeginPlay();

	InitVFX();
	InitAudio();
}

void AGrenadeLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTowardTarget(DeltaTime, 25.0f);
}

void AGrenadeLauncher::Fire()
{
	Super::Fire();

	if(bCanFire)
	{
		LaunchGrenade();
		FiringAudioComponent->Activate(true); //play rocket firing sound

		bCanFire = false;

		GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &AGrenadeLauncher::ResetFire, 1*FireRate, false);
	}
}

void AGrenadeLauncher::LaunchGrenade()
{
	if(ProjectileClass)
	{
		FVector StartLocation = WeaponMesh->GetSocketLocation(TEXT("ProjectileSpawnPoint"));
		FRotator StartRotation = WeaponMesh->GetSocketRotation(TEXT("ProjectileSpawnPoint"));
		FVector LaunchVelocity;
		
		DrawDebugSphere(
		GetWorld(),        // UWorld reference
		TargetPosition,    // Position to draw the sphere
		20.0f,             // Sphere radius
		12,                // Number of segments (higher = smoother)
		FColor::Red,       // Sphere color
		false,             // Persistent (false = disappears after duration)
		2.0f               // Duration (2 seconds)
	);

		UGameplayStatics::FSuggestProjectileVelocityParameters LaunchParams(GetWorld(), StartLocation, TargetPosition, LaunchSpeed);
		LaunchParams.TraceOption = ESuggestProjVelocityTraceOption::DoNotTrace;
		LaunchParams.OverrideGravityZ=Gravity;

		bool bSuccess = UGameplayStatics::SuggestProjectileVelocity(LaunchParams, LaunchVelocity);

		if (!bSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to calculate projectile trajectory!"));

			FVector Direction = (TargetPosition - StartLocation).GetSafeNormal();
			LaunchVelocity = Direction * LaunchSpeed;
			LaunchVelocity.Z = FMath::Max(LaunchVelocity.Z, LaunchSpeed * 0.5f);
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AGrenadeProjectile* Grenade = GetWorld()->SpawnActor<AGrenadeProjectile>(ProjectileClass, StartLocation, StartRotation, SpawnParams);

		if(Grenade) Grenade->GetProjectileMovementComponent()->Velocity = LaunchVelocity;
		
	}
}

void AGrenadeLauncher::MoveTowardTarget(float DeltaTime, float InterpSpeed)
{
	if(OwnerReference)
	{
		if(!OwnerReference->GetCurrentCameraLockSetting())
		{
			if(PlayerController)
			{
				FVector2D MousePosition;
				PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
				FVector WorldLocation;
				FVector WorldDirection;

				if(PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection))
				{
					FVector TraceEnd = WorldLocation + WorldDirection * 10000.0f;
			
					FHitResult HitResult;
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(this); //Ignore Weapon
					Params.AddIgnoredActor(GetOwner()); //Ignore Weapon holder

					bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TraceEnd, ECC_Visibility, Params);
		
					FRotator CurrentRotation = WeaponMesh->GetBoneRotationByName(TEXT("GunRotator"), EBoneSpaces::WorldSpace);
					if(bHit)
					{
						FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("ProjectileSpawnPoint")), HitResult.ImpactPoint);
						LookAtRotation = ClampTargetRotation(LookAtRotation, GetOwner(), 360.0f, 20.0f);
						FRotator TargetRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, InterpSpeed);
						WeaponMesh->SetBoneRotationByName(TEXT("GunRotator"), TargetRotation, EBoneSpaces::WorldSpace);
					}

					else
					{
						FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("ProjectileSpawnPoint")), TraceEnd);
						LookAtRotation = ClampTargetRotation(LookAtRotation, GetOwner(), 360.0f, 20.0f);
						FRotator TargetRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, InterpSpeed);
						WeaponMesh->SetBoneRotationByName(TEXT("GunRotator"), TargetRotation, EBoneSpaces::WorldSpace);
					}
					TargetPosition = HitResult.Location;
				}
			}
		}
		else if(OwnerReference->GetCurrentCameraLockSetting())
		{
			if(PlayerController)
			{
				FVector WorldLocation;
				FVector WorldDirection;

				if(PlayerController->DeprojectScreenPositionToWorld(OwnerReference->GetCurrentTargetScreenPosition().X, OwnerReference->GetCurrentTargetScreenPosition().Y, WorldLocation, WorldDirection))
				{
					FVector TraceEnd = WorldLocation + WorldDirection * 10000.0f;
			
					FHitResult HitResult;
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(this); //Ignore Weapon
					Params.AddIgnoredActor(GetOwner()); //Ignore Weapon holder

					bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TraceEnd, ECC_Visibility, Params);
		
					FRotator CurrentRotation = WeaponMesh->GetBoneRotationByName(TEXT("GunRotator"), EBoneSpaces::WorldSpace);
					if(bHit)
					{
						FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("ProjectileSpawnPoint")), HitResult.ImpactPoint);
						LookAtRotation = ClampTargetRotation(LookAtRotation, GetOwner(), 360.0f, 20.0f);
						FRotator TargetRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, InterpSpeed);
						WeaponMesh->SetBoneRotationByName(TEXT("GunRotator"), TargetRotation, EBoneSpaces::WorldSpace);
					}

					else
					{
						FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(WeaponMesh->GetSocketLocation(TEXT("ProjectileSpawnPoint")), WorldLocation);
						LookAtRotation = ClampTargetRotation(LookAtRotation, GetOwner(), 360.0f, 20.0f);
						FRotator TargetRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, InterpSpeed);
						WeaponMesh->SetBoneRotationByName(TEXT("GunRotator"), TargetRotation, EBoneSpaces::WorldSpace);
					}

					TargetPosition = HitResult.Location;
				}
		
		
			}
		}
	}
}

void AGrenadeLauncher::InitVFX() const
{
	
}

void AGrenadeLauncher::InitAudio() const
{
	if(FiringAudioComponent)
	{
		FiringAudioComponent->SetActive(true);
		FiringAudioComponent->Stop();
	}
}

