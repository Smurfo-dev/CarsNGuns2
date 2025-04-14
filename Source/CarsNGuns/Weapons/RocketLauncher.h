// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "RocketLauncher.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API ARocketLauncher : public ABaseWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARocketLauncher();

	virtual float GetResourceBarValue() const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Fire() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Launcher")
	TSubclassOf<class ABaseProjectile> ProjectileClass;

	//AudioComponent
	UPROPERTY(Category="Audio", EditDefaultsOnly, BlueprintReadOnly)
	UAudioComponent* FiringAudioComponent;

private:
	void LaunchRocket();

	void MoveTowardTarget(float DeltaTime, float InterpSpeed);

	void InitVFX() const;

	void InitAudio() const;

	float ElapsedTimeSinceLastShot = FLT_MAX; // Time that has passed since the last shot
	
	float CurrentProgressBarValue = 1.0f;  // Current value of the progress bar (0 to 1)
};
