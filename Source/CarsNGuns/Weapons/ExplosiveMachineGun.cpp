// Fill out your copyright notice in the Description page of Project Settings.


#include "CarsNGuns/Weapons/ExplosiveMachineGun.h"
#include "CarsNGuns/Vehicles/BasePhysicsVehiclePawn.h"
#include "Kismet/GameplayStatics.h"

void AExplosiveMachineGun::PerformGunTrace(const FVector& BarrelLocation, const FRotator& BarrelRotation,
                                           FHitResult HitResult, const FCollisionQueryParams& Params, const FVector& HitFromDirection)
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
			UGameplayStatics::ApplyRadialDamage(this, BaseDamage * *OwnerReference->GetUpgradeHandlerComponent()->StatConfig.DamageMultipliers.Find(DamageType), GetActorLocation(), ExplosiveDamageRadius, DamageTypeClass, TArray<AActor*>(), this, GetInstigatorController(), false);
			//Can add damage prevention channel
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticleSystem, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), FVector(ExplosionScaleFactor));
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSoundCue, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());

			if(bDrawExplosionDebugSphere) DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosiveDamageRadius, 12, FColor::Red, false, 5.0f, 0, 1.0f);
		}
	}
	else //If nothing is hit
	{
		UE_LOG(LogTemp, Warning, TEXT("Aint shooting shit"));
	}
}
