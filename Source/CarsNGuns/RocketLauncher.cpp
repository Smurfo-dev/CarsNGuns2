// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketLauncher.h"

#include "BaseProjectile.h"
#include "PlayerVehicleBase.h"
#include "Components/AudioComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

ARocketLauncher::ARocketLauncher()
{
	//Create Audio Component(s)
	FiringAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FiringAudioComponent"));
}

void ARocketLauncher::BeginPlay()
{
	Super::BeginPlay();

	InitVFX();
	InitAudio();
}

void ARocketLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTowardTarget(DeltaTime, 25.0f);
}

void ARocketLauncher::Fire()
{
	Super::Fire();
	
	if(bCanFire)
	{
		LaunchRocket();
		FiringAudioComponent->Activate(true); //play rocket firing sound

		bCanFire = false;

		GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &ARocketLauncher::ResetFire, 1*FireRate, false);
	}
}


void ARocketLauncher::LaunchRocket()
{
	if(ProjectileClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = GetOwner();

		if(ABaseProjectile* Projectile = GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, WeaponMesh->GetSocketLocation(TEXT("ProjectileSpawnPoint")), WeaponMesh->GetSocketRotation(TEXT("ProjectileSpawnPoint")), Params))
		{
			//Initialize projectile properties?
			
		}
	}
}

void ARocketLauncher::MoveTowardTarget(float DeltaTime, float InterpSpeed)
{
	if(PlayerReference)
	{
		if(!PlayerReference->GetCurrentCameraLockSetting())
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
				}
			}
		}
		else if(PlayerReference->GetCurrentCameraLockSetting())
		{
			if(PlayerController)
			{
				FVector WorldLocation;
				FVector WorldDirection;

				if(PlayerController->DeprojectScreenPositionToWorld(PlayerReference->GetCurrentTargetScreenPosition().X, PlayerReference->GetCurrentTargetScreenPosition().Y, WorldLocation, WorldDirection))
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
				}
		
		
			}
		}
	}
}

void ARocketLauncher::InitVFX() const
{
	
}

void ARocketLauncher::InitAudio() const
{
	if(FiringAudioComponent)
	{
		FiringAudioComponent->SetActive(true);
		FiringAudioComponent->Stop();
	}
}
