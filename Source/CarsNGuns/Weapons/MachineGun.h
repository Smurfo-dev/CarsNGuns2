// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "GameFramework/Actor.h"
#include "MachineGun.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API AMachineGun : public ABaseWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMachineGun();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Fire() override;

	virtual void StopFire() override;

	virtual float GetResourceBarValue() const override;

	UPROPERTY(EditAnywhere, Category="VFX")
	UMaterialInterface* BulletHoleDecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	float MaxRecoilAngle = 3.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//VFX Systems
	UPROPERTY(Category="VFX", EditDefaultsOnly, BlueprintReadOnly)
	class UNiagaraComponent* TracerNiagaraComponentLeft;

	UPROPERTY(Category="VFX", EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraComponent* TracerNiagaraComponentRight;

	UPROPERTY(Category="VFX", EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraComponent* MuzzleFlashNiagaraComponentLeft;

	UPROPERTY(Category="VFX", EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraComponent* MuzzleFlashNiagaraComponentRight;

	UPROPERTY(Category="VFX", EditDefaultsOnly, BlueprintReadOnly)
	class UNiagaraSystem* TracerNiagaraSystem;

	UPROPERTY(Category="VFX", EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* MuzzleFlashNiagaraSystem;

	UPROPERTY(Category="VFX", EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* BulletHoleDebrisNiagaraSystem;

	//AudioComponent
	UPROPERTY(Category="Audio", EditDefaultsOnly, BlueprintReadOnly)
	UAudioComponent* FiringAudioComponent;

private:
	
	void PerformHitScan();

	void PerformGunTrace(const FVector& BarrelLocation, const FRotator& BarrelRotation, FHitResult HitResult, const FCollisionQueryParams& Params, const FVector& HitFromDirection);
	
	void InitVFX() const;

	void InitAudio() const;

	void MoveGunsTowardTarget(float DeltaTime, float InterpSpeed) const;

	UPROPERTY(EditAnywhere, Category ="Weapon Statistics")
	float MaxRange = 10000.0f; //Max range for hitscan

	UPROPERTY(EditAnywhere, Category ="Weapon Statistics")
	float MaxAmmo = 100;

	UPROPERTY(EditAnywhere, Category ="Weapon Statistics")
	float CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Statistics")
	float AmmoPerShot = 1; // Ammo consumed per shot

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Statistics")
	float ReloadTime = 2.0f; // Time to reload

	FTimerHandle ReloadTimerHandle;
	
	void Reload();

	void FinishReload();
	
	bool bIsWindingDown = false;

	bool bIsReloading  = false;
	
};
