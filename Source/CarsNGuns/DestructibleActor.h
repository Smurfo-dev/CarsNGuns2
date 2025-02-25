// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HealthComponent.h"
#include "GameFramework/Actor.h"
#include "DestructibleActor.generated.h"

UCLASS()
class CARSNGUNS_API ADestructibleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructibleActor();

	UFUNCTION()
	void OnDeath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Mesh")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category="Health")
	UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category="Armor")
	EArmorLevel ArmorLevel = EArmorLevel::None;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
