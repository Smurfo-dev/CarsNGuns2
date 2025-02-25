// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ARocketProjectile::ARocketProjectile()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->MaxSpeed = MaxSpeed;
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;

	RocketTrailNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RocketTrailNiagaraSystem"));
	RocketTrailNiagaraComponent->SetupAttachment(RootComponent);

	InitialLifeSpan = 10.0f;
}

void ARocketProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorHit.AddDynamic(this, &ARocketProjectile::OnImpact);
	RocketTrailNiagaraComponent->SetAsset(RocketTrailNiagaraSystem);
	RocketTrailNiagaraComponent->Activate();
}


void ARocketProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARocketProjectile::OnImpact(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& HitResult)
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
