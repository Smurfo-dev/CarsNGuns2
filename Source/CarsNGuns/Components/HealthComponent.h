// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

UENUM(BlueprintType)
enum class EArmorLevel : uint8
{
	None  UMETA(DisplayName = "None"),
	Light  UMETA(DisplayName = "Light"),
	Medium UMETA(DisplayName = "Medium"),
	Heavy  UMETA(DisplayName = "Heavy")
};

// Declare a delegate for health reaching zero
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARSNGUNS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UPROPERTY(Category=Health, EditAnywhere)
	float MaxHealth = 1000.0f;

	UPROPERTY(VisibleAnywhere, Category="Armor")
	TMap<EArmorLevel, float> ArmorMap;

	//Initialize default active armor
	UPROPERTY()
	EArmorLevel ActiveArmorLevel = EArmorLevel::None;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugDamage = true;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser);
	
	UPROPERTY(Category=Health, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	float CurrentHealth = 100.f;
};
