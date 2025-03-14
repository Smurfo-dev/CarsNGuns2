// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"

void AEnemyManager::AddEnemy(AActor* NewEnemy)
{
	if (NewEnemy && !Enemies.Contains(NewEnemy))
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding Enemy"));
		Enemies.Add(NewEnemy);
	}
}

void AEnemyManager::RemoveEnemy(AActor* EnemyToRemove)
{
	UE_LOG(LogTemp, Warning, TEXT("Removing Enemy"));
	Enemies.Remove(EnemyToRemove);
}

void AEnemyManager::ClearEnemies()
{
	if(!Enemies.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Clearing Enemy Array"));
		Enemies.Empty();
	}
	
}
