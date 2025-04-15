// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None,
	MachineGun,
	LaserRifle,
	RocketLauncher,
	GrenadeLauncher,
};

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	Bullet,
	Explosive,
	Special,
};

UENUM(BlueprintType)
enum class EStatEnhancementType : uint8
{
	Damage UMETA(DisplayName = "Damage"),
	FireRate UMETA(DisplayName = "Fire Rate"),
	ReloadSpeed UMETA(DisplayName = "Reload Speed"),
	// mer typer perhabs
};

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
	virtual float GetResourceBarValue() const;

	UFUNCTION()
	FString GetWeaponID();

	UPROPERTY()
	class ABasePhysicsVehiclePawn* OwnerReference;

	UFUNCTION(BlueprintCallable, Category="UI")
	UTexture2D* GetWeaponIcon() const {return WeaponIcon;}

	UFUNCTION(Blueprintable)
	EWeaponType GetWeaponType() const
	{
		return WeaponType;
	}
	UFUNCTION(Blueprintable)
	EDamageType GetDamageType() const
	{
		return DamageType;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category ="Weapon Statistics")
	float BaseDamage = 30.0f; //Base Damage amount per bullet

	UPROPERTY(EditAnywhere, Category ="Weapon Statistics", meta=(ToolTip="Calculated as 1 seconds x Fire Rate multiplier"))
	float BaseFireRate = 1.0f; //Rate of fire multiplier (bars)

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Statistics")
	float BaseReloadTime = 2.0f; // Time to reload

	UPROPERTY(VisibleAnywhere)
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere)
	EDamageType DamageType;
	
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
	TSubclassOf<class UDamageType> DamageTypeClass; //Damage type

	bool bCanFire;

	FTimerHandle FireRateTimerHandle;

	void ResetFire();
	
	void InitializePlayerController();
	
	FRotator ClampTargetRotation(FRotator TargetRotation, const AActor* PlayerActor, float YawLimit, float PitchLimit) const;
};
