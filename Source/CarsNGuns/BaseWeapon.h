// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS()
class CARSNGUNS_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Fire();

	virtual void StopFire();

	UFUNCTION()
	FString GetWeaponID();

	UPROPERTY()
	class ABasePhysicsVehiclePawn* OwnerReference;

	UFUNCTION(BlueprintCallable, Category="UI")
	UTexture2D* GetWeaponIcon() const {return WeaponIcon;};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category=UI)
	UTexture2D* WeaponIcon;

	UPROPERTY(EditDefaultsOnly, Category="Info")
	FString WeaponName = FString("Default");

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* WeaponRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class UPoseableMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category="Weapon Statistics")
	TSubclassOf<class UDamageType> DamageType; //Damage type

	UPROPERTY(EditAnywhere, Category ="Weapon Statistics", meta=(ToolTip="Calculated as 1 seconds x Fire Rate multiplier"))
	float FireRate = 1.0f; //Rate of fire multiplier (bars)

	bool bCanFire;

	FTimerHandle FireRateTimerHandle;

	void ResetFire();
	
	void InitializePlayerController();
	
	FRotator ClampTargetRotation(FRotator TargetRotation, const AActor* PlayerActor, float YawLimit, float PitchLimit) const;
};
