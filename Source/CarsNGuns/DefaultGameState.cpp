// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameState.h"

#include "EnemyVehicleBase.h"
#include "EnemyManager.h"
#include "Kismet/GameplayStatics.h"

ADefaultGameState::ADefaultGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADefaultGameState::BeginPlay()
{
	Super::BeginPlay();

	// Create and initialize the EnemyManager.
	EnemyManager = NewObject<UEnemyManager>(this);
}

void ADefaultGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADefaultGameState::PopulateEnemies() const
{
	if(EnemyManager)
	{
		//Clear enemy list initially
		EnemyManager->ClearEnemies();
		
		UE_LOG(LogTemp, Warning, TEXT("EnemyManager Exists"));
		TArray<AActor*> FoundEnemies;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyVehicleBase::StaticClass(), FoundEnemies);
		for(AActor* Enemy : FoundEnemies)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found enemy: %s, Class: %s"), *Enemy->GetName(), *Enemy->GetClass()->GetName());
			EnemyManager->AddEnemy(Enemy);
		}
	}
}

