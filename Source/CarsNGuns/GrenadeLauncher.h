// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "GrenadeLauncher.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API AGrenadeLauncher : public ABaseWeapon
{
	GENERATED_BODY()

public:
	AGrenadeLauncher();

	virtual void Tick(float DeltaTime) override;

	virtual void Fire() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Launcher")
	TSubclassOf<class ABaseProjectile> ProjectileClass;

	//AudioComponent
	UPROPERTY(Category="Audio", EditDefaultsOnly, BlueprintReadOnly)
	UAudioComponent* FiringAudioComponent;

private:
	void LaunchGrenade();

	void MoveTowardTarget(float DeltaTime, float InterpSpeed);

	void InitVFX() const;

	void InitAudio() const;

	//Adjustable
	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float LaunchSpeed = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float Gravity = -980.0f;

	UPROPERTY()
	FVector TargetPosition;
	
};
