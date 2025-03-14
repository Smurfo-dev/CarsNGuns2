// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AGrenadeProjectile::AGrenadeProjectile()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->MaxSpeed = MaxSpeed;
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;

	InitialLifeSpan = 5.0f;
}

void AGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();

	OnActorHit.AddDynamic(this, &AGrenadeProjectile::OnImpact);
}

void AGrenadeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGrenadeProjectile::OnImpact(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse,
	const FHitResult& HitResult)
{
	if(DamageRadius > 0.0f)
	{
		UGameplayStatics::ApplyRadialDamage(this, Damage, GetActorLocation(), DamageRadius, DamageType, TArray<AActor*>(), this, GetInstigatorController(), false);
		//Can add damage prevention channel
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticleSystem, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), FVector(ExplosionScaleFactor));
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSoundCue, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());

		if(bDrawExplosionDebugSphere) DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 12, FColor::Red, false, 5.0f, 0, 1.0f);
		
	}
	Destroy(); //Kill after impact 
}

UProjectileMovementComponent* AGrenadeProjectile::GetProjectileMovementComponent() const
{
	return ProjectileMovementComponent;
}