// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarsNGuns/Components/MissionUpgradeComponent.h"
#include "BaseMission.generated.h"

UENUM(BlueprintType)
enum class EMissionState : uint8
{
	Inactive,
	Active,
	Completed,
	Failed
};

UCLASS()
class CARSNGUNS_API ABaseMission : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseMission();

	virtual void StartEvent();
	virtual void EndEvent(bool bSuccess);

	virtual void ActivateCheckpoint();

	UPROPERTY(EditAnywhere, Category="Mission Reward")
	UMissionUpgradeComponent* MissionUpgradeComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Marker")
	FLinearColor MarkerColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, Category="Marker")
	class UWidgetComponent* MissionMarkerWidgetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Marker")
	class USphereComponent* MissionTriggerZone;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Marker")
	UStaticMeshComponent* MissionTriggerMesh;

	void SetMissionState(const EMissionState NewState)
	{
		MissionState = NewState;
		OnMissionStateChanged(NewState);
	}

	EMissionState GetMissionState() const
	{
		return MissionState;
	}

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UMissionInfoWidget> MissionInfoMenuWidgetClass;

	UPROPERTY()
	UMissionInfoWidget* CurrentMissionInfoWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUpgradeSelectionWidget> UpgradeSelectionWidgetClass;

	UPROPERTY()
	UUpgradeSelectionWidget* UpgradeSelectionWidget;

	FString GetMissionStateAsString(EMissionState State) const
	{
		switch (State)
		{
		case EMissionState::Inactive:  return TEXT("Inactive");
		case EMissionState::Active:    return TEXT("Active");
		case EMissionState::Completed: return TEXT("Completed");
		case EMissionState::Failed:    return TEXT("Failed");
		default: return TEXT("Unknown");
		}
	}

	UFUNCTION(BlueprintCallable, Category="Mission Info")
	void PrintMissionInfo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	EMissionState MissionState;

	UPROPERTY()
	class UMissionMarkerWidget* MissionMarkerWidget;

	UPROPERTY()
	class ADefaultGameState* DefaultGameState;

	UPROPERTY()
	class UDefaultGameInstance* DefaultGameInstance;

	UPROPERTY(VisibleAnywhere)
	float MissionStartTime = 0.0f;

	UPROPERTY()
	bool bMissionStarted = false;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float GetMissionTime();
	
	UFUNCTION()
	void ApplyUpgrade(int32 UpgradeIndex);

private:
	UFUNCTION()
	void OnPlayerEnterMissionArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerExitMissionArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnMissionStateChanged(EMissionState NewState);

	UPROPERTY()
	class AMyPlayerController* PlayerController;

	UFUNCTION()
	void ShowMissionInfo();

	UFUNCTION()
	void HideMissionInfo();
	
	UPROPERTY()
	class ABasePhysicsVehiclePawn* PlayerReference;
	
	UFUNCTION()
	void ShowMissionRewards(const TArray<FUpgrade>& Upgrades);
	
	UFUNCTION()
	void DisableMission();

	UFUNCTION()
	void EnableMission();

};
