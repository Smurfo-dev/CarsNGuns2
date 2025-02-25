// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleActor.h"

// Sets default values
ADestructibleActor::ADestructibleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMeshComponent;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

}

// Called when the game starts or when spawned
void ADestructibleActor::BeginPlay()
{
	Super::BeginPlay();

	//This must be done for every user of healthcomponent, probably
	if(HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ADestructibleActor::OnDeath);
		HealthComponent->ActiveArmorLevel = ArmorLevel;
	}
}

// Called every frame
void ADestructibleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADestructibleActor::OnDeath()
{
	//What to do on death
	//crumble destructible mesh?
	Destroy();
}

