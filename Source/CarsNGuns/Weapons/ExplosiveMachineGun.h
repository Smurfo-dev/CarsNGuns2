// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarsNGuns/Weapons/MachineGun.h"
#include "ExplosiveMachineGun.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API AExplosiveMachineGun : public AMachineGun
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int ExplosionScaleFactor = 5;

	UPROPERTY(EditAnywhere, Category ="Debug")
	bool bDrawExplosionDebugSphere = false;
	
	UPROPERTY(EditAnywhere, Category ="Weapon Damage")
	float ExplosiveDamageRadius = 50.0f;

	UPROPERTY(Category="VFX", EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* ExplosionParticleSystem;

	UPROPERTY(Category="SFX", EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ExplosionSoundCue;

protected:
	virtual void PerformGunTrace(const FVector& BarrelLocation, const FRotator& BarrelRotation, FHitResult HitResult, const FCollisionQueryParams& Params, const FVector& HitFromDirection) override;

	
	
};
