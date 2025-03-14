// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class CARSNGUNS_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, Category ="Projectile Statistics")
	float Damage = 1.0f; //Damage amount per projectile

	UPROPERTY(EditAnywhere, Category ="Projectile Statistics")
	float MaxSpeed = 8000.0f; //Damage amount per projectile

	UPROPERTY(EditAnywhere, Category ="Projectile Statistics")
	float InitialSpeed = 4000.0f; //Damage amount per projectile

	UPROPERTY(EditAnywhere, Category="Projectile Statistics")
	TSubclassOf<class UDamageType> DamageType; //Damage type

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
