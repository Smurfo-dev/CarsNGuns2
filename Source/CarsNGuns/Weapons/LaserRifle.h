// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "LaserRifle.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API ALaserRifle : public ABaseWeapon
{
	GENERATED_BODY()

public:
	ALaserRifle();

	virtual void Tick(float DeltaTime) override;

	virtual void Fire() override;

	virtual void StopFire() override;

	virtual float GetResourceBarValue() const override;

	UPROPERTY(Category="VFX", EditDefaultsOnly, BlueprintReadOnly)
	class UNiagaraComponent* LaserBeamNiagaraComponent;

	UPROPERTY(Category="VFX", EditAnywhere)
	class UNiagaraSystem* LaserBeamNiagaraSystem;

	UPROPERTY(Category="VFX", EditAnywhere)
	UNiagaraSystem* LaserBeamImpactNiagaraSystem;
	
protected:
	virtual void BeginPlay() override;

	//AudioComponent
	UPROPERTY(Category="Audio", EditDefaultsOnly, BlueprintReadOnly)
	UAudioComponent* FiringAudioComponent;

	UPROPERTY(EditDefaultsOnly, Category=Overheat)
	float MaxHeat = 100.0f; //Max heat before Overheated status

	UPROPERTY(EditDefaultsOnly, Category=Overheat)
	float HeatPerTick = 10.0f; //Heat Added Per Tick

	UPROPERTY(EditDefaultsOnly, Category=Overheat)
	float CooldownTime = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category=Overheat)
	float PassiveCooldownDelayTime = 1.5; //Time of not shooting before passive cooldown
	
	UPROPERTY(EditDefaultsOnly, Category=Overheat)
	float CurrentHeat = 0.0f; //Current Heat Level

	UPROPERTY(EditDefaultsOnly, Category=Overheat)
	float HeatCooldownRate = 5.0f; //Heat removed per tick

	UPROPERTY(EditDefaultsOnly, Category=Overheat)
	float PassiveHeatCooldownTickRate = 0.1; //Heat removed per tick

	bool bIsOverheated = false;

	FTimerHandle OverheatTimerHandle;
	FTimerHandle PassiveCooldownTimerHandle;
	FTimerHandle CooldownDelayTimerHandle;

private:
	void PerformHitScan();

	void PerformBeamScan();

	void PerformGunTrace(const FVector& BarrelLocation, const FRotator& BarrelRotation, FHitResult HitResult, const FCollisionQueryParams& Params, const FVector& HitFromDirection);

	void PerformBeamTrace(const FVector& BarrelLocation, const FRotator& BarrelRotation, FHitResult HitResult, const FCollisionQueryParams& Params, const FVector& HitFromDirection);

	void MoveTowardTarget(float DeltaTime, float InterpSpeed);

	void InitVFX() const;

	void InitAudio() const;
	
	void CoolDown();

	void StartPassiveCooldown();

	void CooldownTick();
	
	UPROPERTY(EditAnywhere, Category ="Weapon Statistics")
	float MaxRange = 10000.0f; //Max range for hitscan

	bool bIsWindingDown = false;

	float LastSpawnTime = 0.0f;
	
};
