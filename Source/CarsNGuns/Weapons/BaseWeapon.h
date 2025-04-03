// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	MachineGun,
	LaserRifle,
	RocketLauncher,
	GrenadeLauncher,
};

UENUM(BlueprintType)
enum class EUpgradeDamageType : uint8
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

	UFUNCTION()
	void ApplyEnhancement(EStatEnhancementType TypeToEnhance, float EnhancementValue);

	void SetDamage(const float Value)
	{
		Damage *= 1.0f + Value / 100.0f;
	}

	void SetFireRate(const float Value)
	{
		FireRate *= 1.0f - Value / 100.0f;
	}

	void SetReloadSpeed(const float Value)
	{
		ReloadTime *= 1.0f - Value / 100.0f;
	}

	UFUNCTION(Blueprintable)
	EWeaponType GetWeaponType() const
	{
		return WeaponType;
	}
	UFUNCTION(Blueprintable)
	EUpgradeDamageType GetUpgradeDamageType() const
	{
		return UpgradeDamageType;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category ="Weapon Statistics")
	float BaseDamage = 30.0f; //Base Damage amount per bullet

	float Damage; 

	UPROPERTY(EditAnywhere, Category ="Weapon Statistics", meta=(ToolTip="Calculated as 1 seconds x Fire Rate multiplier"))
	float BaseFireRate = 1.0f; //Rate of fire multiplier (bars)
	
	float FireRate;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Statistics")
	float BaseReloadTime = 2.0f; // Time to reload
	
	float ReloadTime;

	UPROPERTY(VisibleAnywhere)
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere)
	EUpgradeDamageType UpgradeDamageType;
	
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

	bool bCanFire;

	FTimerHandle FireRateTimerHandle;

	void ResetFire();
	
	void InitializePlayerController();
	
	FRotator ClampTargetRotation(FRotator TargetRotation, const AActor* PlayerActor, float YawLimit, float PitchLimit) const;
};
