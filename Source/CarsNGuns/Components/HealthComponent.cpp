// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Armor Map Defaults
	ArmorMap.Add(EArmorLevel::None, 0.0f);
	ArmorMap.Add(EArmorLevel::Light, 20.0f);
	ArmorMap.Add(EArmorLevel::Medium, 200.0f);
	ArmorMap.Add(EArmorLevel::Heavy, 400.0f);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);
	
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
{
	
	if(Damage <= ArmorMap[ActiveArmorLevel]) return;

	//Nerf bullet damage (temp perchance)
	if(DamageType->GetName()==TEXT("Default__BP_BulletDamageType_C")) Damage/=20;

	CurrentHealth -=Damage;

	if(bDebugDamage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Damage Taken: %.0f"), Damage);
		UE_LOG(LogTemp, Warning, TEXT("Armor Level: %.f"), ArmorMap[ActiveArmorLevel]);
		UE_LOG(LogTemp, Warning, TEXT("Damage Type: %s"), *DamageType->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Current Health: %.f"), CurrentHealth);
	}
	
	
	if(CurrentHealth <= 0)
	{
		OnDeath.Broadcast();
	}
}

