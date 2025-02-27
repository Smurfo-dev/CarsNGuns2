// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemyAIController.generated.h"

/**
 * 
 */

enum class EAIState
{
	Follow UMETA(DisplayName = "Follow"),
	Chase UMETA(DisplayName = "Chase"),
	Torpedo UMETA(DisplayName = "Torpedo"),
	Shooting UMETA(DisplayName = "Shooting"),
	Idle UMETA(DisplayName = "Idle"),
	Dead UMETA(DisplayName = "Dead")
};

UCLASS()
class CARSNGUNS_API ABaseEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseEnemyAIController();

	//begin play
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

protected:
	
	virtual void Follow();

	void Chase();

	void Idle();

	void Dead();

	virtual void Torpedo();

	virtual void Shooting();
	
	EAIState CurrentState;

	virtual void TransitionFromFollow();

	UPROPERTY()
	class AEnemyVehicleBase* EnemyVehicleReference = nullptr;

	void SetState(EAIState NewState);

private:

	FString GetEnemyStateAsString() const;
	
	UPROPERTY()
	class APlayerVehicleBase* PlayerReference = nullptr;

	float FollowStateTimeElapsed = 0.0f;

	UPROPERTY(EditAnywhere, Category = AI)
	float FollowStateDuration = 3.0f; //Time to follow before entering follow-up state

	UPROPERTY(EditAnywhere, Category = AI)
	float BrakingRadius = 600.0f; //Distance where AI will break to not overtake player

	UPROPERTY(EditAnywhere, Category = AI)
	float FollowRadius = 2000.0f; //Distance AI will try to follow player at

	void InitializeReferences();
};
