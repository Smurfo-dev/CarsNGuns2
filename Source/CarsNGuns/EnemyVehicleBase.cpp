// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyVehicleBase.h"
#include "BaseWeapon.h"
#include "DefaultGameInstance.h"
#include "Kismet/GameplayStatics.h"

AEnemyVehicleBase::AEnemyVehicleBase()
{
	//Init Health Component
	if(HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &AEnemyVehicleBase::OnDeath);
		HealthComponent->ActiveArmorLevel = ArmorLevel;
	}
	
	if(AIControllerClass)
	{
		AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	}
}

void AEnemyVehicleBase::OnDeath()
{
	UE_LOG(LogTemp, Error, TEXT("Enemy just fucking died"));
	if(CollisionBoxPrimitive && !bIsDead)
	{
		const float BaseImpulse = 750.0f;
		const float RandomRange = 200.0f;
		const float MaxAngularImpulse = 500.0f;
		FVector RandomImpulse = FVector(FMath::RandRange(-RandomRange, RandomRange), FMath::RandRange(-RandomRange, RandomRange), BaseImpulse + FMath::RandRange(0.0f, RandomRange));
		FVector RandomAngularImpulse = FVector(FMath::RandRange(-MaxAngularImpulse, MaxAngularImpulse), FMath::RandRange(-MaxAngularImpulse, MaxAngularImpulse), FMath::RandRange(-MaxAngularImpulse, MaxAngularImpulse));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticleSystem, GetActorLocation(), GetActorRotation(), FVector(DeathExplosionScaleFactor));
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSoundCue, GetActorLocation(), GetActorRotation());
		CollisionBoxPrimitive->AddImpulse(RandomImpulse, NAME_None, true);
		CollisionBoxPrimitive->AddAngularImpulseInDegrees(RandomAngularImpulse, NAME_None, true);
		bIsDead = true;

		//Remove from enemy manager
		UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (GameInstance && GameInstance->GetEnemyManager())
		{
			GameInstance->GetEnemyManager()->RemoveEnemy(this);
		}
		
		GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AEnemyVehicleBase::DestroyActor, 3.0f, false); //Kill after time
	}
}

void AEnemyVehicleBase::DestroyActor()
{
	PrimaryWeapon->Destroy();
	SecondaryWeapon->Destroy();
	Destroy();
}
