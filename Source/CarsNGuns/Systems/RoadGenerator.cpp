// Fill out your copyright notice in the Description page of Project Settings.


#include "CarsNGuns/Systems/RoadGenerator.h"

// Sets default values
ARoadGenerator::ARoadGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	RootComponent = SplineComponent;

}

void ARoadGenerator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	GenerateRoad();
}

void ARoadGenerator::GenerateRoad()
{
	if (RoadMesh && SplineComponent)
	{
		TArray<USceneComponent*> AttatchedComponents;
		GetComponents<USceneComponent>(AttatchedComponents);
		for (auto Component : AttatchedComponents)
		{
			if (Component != SplineComponent) Component->DestroyComponent();
		}

		//Placera meshes enligt spline
		const float SplineLength = SplineComponent->GetSplineLength();
		const int NumOfMeshes = FMath::CeilToInt(SplineLength / MeshSpacing);

		for (int i = 0; i < NumOfMeshes; i++)
		{
			const float Distance = i * MeshSpacing;
			const FVector Location = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
			const FRotator Rotation = SplineComponent->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

			UStaticMeshComponent* RoadSegment = NewObject<UStaticMeshComponent>(this);
			RoadSegment->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepWorldTransform);
			RoadSegment->SetStaticMesh(RoadMesh);
			RoadSegment->SetWorldLocation(Location);
			RoadSegment->SetWorldRotation(Rotation);
			RoadSegment->RegisterComponent();
		}
	}
}
