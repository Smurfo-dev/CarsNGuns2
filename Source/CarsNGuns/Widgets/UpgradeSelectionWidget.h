// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradeSelectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class CARSNGUNS_API UUpgradeSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Option1Image;

	UPROPERTY(meta = (BindWidget))
	UImage* Option2Image;

	UPROPERTY(meta = (BindWidget))
	UImage* Option3Image;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Option1DisplayName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Option2DisplayName;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Option3DisplayName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Option1Description;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Option2Description;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Option3Description;

	UPROPERTY(meta = (BindWidget))
	class UButton* Option1Button;

	UPROPERTY(meta = (BindWidget))
	UButton* Option2Button;

	UPROPERTY(meta = (BindWidget))
	UButton* Option3Button;
	
};
