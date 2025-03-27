// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "RoadGenerator.generated.h"

UCLASS()
class CARSNGUNS_API ARoadGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoadGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road")
	USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road")
	UStaticMesh* RoadMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road")
	float MeshSpacing = 500.0f; //Justera enligt mesh längd

	void GenerateRoad();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

};
