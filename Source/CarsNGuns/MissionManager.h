// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissionManager.generated.h"

USTRUCT(BlueprintType)
struct FMissionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission Data");
	FName MissionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission Data");
	TArray<TSoftObjectPtr<AActor>> Checkpoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission Data")
	float TimeLimit;
	
};

UCLASS()
class CARSNGUNS_API AMissionManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMissionManager();

	UFUNCTION(BlueprintCallable)
	void StartEvent(FName EventID);

	UFUNCTION(BlueprintCallable)
	void OnCheckPointReached();

	void EndEvent(FName EventID);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite)
	TSoftObjectPtr<AActor> ActiveCheckpoint;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentCheckpointIndex;

	UPROPERTY(BlueprintReadWrite)
	bool bMissionActive = false;

	UPROPERTY(EditAnywhere, Category = "Mission Events")
	TMap<FName, FMissionData> MissionDatabase;

private:
	FMissionData* CurrentMission = nullptr;

};
