// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BaseWeapon.h"
#include "DefaultGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TArray<TSubclassOf<ABaseWeapon>> AvailableWeapons;


protected:
	virtual void Init() override;
	
};

