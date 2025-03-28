// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseProjectile.h"
#include "GrenadeProjectile.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API AGrenadeProjectile : public ABaseProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGrenadeProjectile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	int ExplosionScaleFactor = 3;

	UProjectileMovementComponent* GetProjectileMovementComponent() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnImpact(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& HitResult);

	UPROPERTY(EditAnywhere, Category ="Projectile Statistics")
	float DamageRadius = 40.0f; //Damage explosion radius

	UPROPERTY(EditAnywhere, Category ="Debug")
	bool bDrawExplosionDebugSphere; //Damage explosion radius
	

	UPROPERTY(Category="VFX", EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* ExplosionParticleSystem;

	UPROPERTY(Category="SFX", EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ExplosionSoundCue;
	
};
