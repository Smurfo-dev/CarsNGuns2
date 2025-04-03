// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "Components/PoseableMeshComponent.h"
#include "GameFramework/Actor.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
    WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
    SetRootComponent(WeaponRoot);
    	
    WeaponMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(WeaponRoot);

}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	bCanFire = true;
	
	ReloadTime = BaseReloadTime;
	FireRate = BaseFireRate;
	Damage = BaseDamage;	

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABaseWeapon::InitializePlayerController);
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseWeapon::Fire()
{
	
}

void ABaseWeapon::StopFire()
{
	
}

float ABaseWeapon::GetResourceBarValue() const
{
	return 0;
}

FString ABaseWeapon::GetWeaponID()
{
	return WeaponName;
}


void ABaseWeapon::ResetFire()
{
	bCanFire = true;
}

void ABaseWeapon::InitializePlayerController()
{
	AController* InstigatorController = GetOwner()->GetInstigatorController();
	if (InstigatorController)
	{
		PlayerController = Cast<APlayerController>(InstigatorController);
		if (PlayerController)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerController initialized: %s"), *PlayerController->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to cast to APlayerController."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InstigatorController is null."));
	}
}

FRotator ABaseWeapon::ClampTargetRotation(FRotator TargetRotation, const AActor* PlayerActor, float YawLimit, float PitchLimit) const
{
	const FRotator PlayerRotation = PlayerActor->GetActorRotation();
	float YawDifference = TargetRotation.Yaw - PlayerRotation.Yaw;
	float PitchDifference = TargetRotation.Pitch - PlayerRotation.Pitch;

	YawDifference = FMath::UnwindDegrees(YawDifference);
	PitchDifference = FMath::UnwindDegrees(PitchDifference);

	const float ClampedYawDifference = FMath::Clamp(YawDifference, -YawLimit, YawLimit);
	const float ClampedPitchDifference = FMath::Clamp(PitchDifference, -PitchLimit, PitchLimit);

	TargetRotation.Yaw = PlayerRotation.Yaw + ClampedYawDifference;
	TargetRotation.Pitch = PlayerRotation.Pitch + ClampedPitchDifference;

	return TargetRotation;
}

