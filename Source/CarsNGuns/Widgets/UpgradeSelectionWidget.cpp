// Fill out your copyright notice in the Description page of Project Settings.


#include "CarsNGuns/Widgets/UpgradeSelectionWidget.h"
#include "Engine/Texture2D.h"
#include "Components/TextBlock.h"
#include "Misc/FileHelper.h"
#include "CarsNGuns/Player/MyPlayerController.h"
#include "Misc/Paths.h"
#include "ImageUtils.h"
#include "Engine/Texture2D.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UUpgradeSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Option1Button) Option1Button->OnClicked.AddDynamic(this, &UUpgradeSelectionWidget::OnOption1Clicked);

	if (Option2Button) Option2Button->OnClicked.AddDynamic(this, &UUpgradeSelectionWidget::OnOption2Clicked);

	if (Option3Button) Option3Button->OnClicked.AddDynamic(this, &UUpgradeSelectionWidget::OnOption3Clicked);
}


void UUpgradeSelectionWidget::OnOption1Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Option 1 Chosen"));
	if (MissionReference)
	{
		MissionReference->ApplyUpgrade(0);
	}
	DisableWidget();
}

void UUpgradeSelectionWidget::OnOption2Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Option 2 Chosen"));
	if (MissionReference)
	{
		MissionReference->ApplyUpgrade(1);
	}
	DisableWidget();
}

void UUpgradeSelectionWidget::OnOption3Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Option 3 Chosen"));
	if (MissionReference)
	{
		MissionReference->ApplyUpgrade(2);
	}
	DisableWidget();
}

void UUpgradeSelectionWidget::InitializeValues(const TArray<FUpgrade>& Upgrades, AMyPlayerController* PlayerControllerRef, ABaseMission* MissionRef)
{
	PlayerController = PlayerControllerRef;
	MissionReference = MissionRef;
	
	if (Upgrades.Num() > 0)
	{
		SetImageFromFile(Upgrades[0].UpgradeIconFilePath, Option1Image);
		Option1DisplayName->SetText(FText::FromString(Upgrades[0].DisplayName));
		Option1Description->SetText(FText::FromString(Upgrades[0].UpgradeDescription));
	}
	if (Upgrades.Num() > 1)
	{
		SetImageFromFile(Upgrades[1].UpgradeIconFilePath, Option2Image);
		Option2DisplayName->SetText(FText::FromString(Upgrades[1].DisplayName));
		Option2Description->SetText(FText::FromString(Upgrades[1].UpgradeDescription));
	}
	if (Upgrades.Num() > 2)
	{
		SetImageFromFile(Upgrades[2].UpgradeIconFilePath, Option3Image);
		Option3DisplayName->SetText(FText::FromString(Upgrades[2].DisplayName));
		Option3Description->SetText(FText::FromString(Upgrades[2].UpgradeDescription));
	}

	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);
		FInputModeGameAndUI InputMode;
		PlayerController->SetInputMode(InputMode);
	}
}

void UUpgradeSelectionWidget::SetImageFromFile(const FString& FilePath, UImage* Image)
{
	if (UTexture2D* LoadedTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FilePath)))
	{
		if (LoadedTexture && Image)
		{
			FSlateBrush Brush;
			Brush.SetResourceObject(LoadedTexture);
			Image->SetDesiredSizeOverride(FVector2D(400,400)); //Make The Upgrade Icons in 400x400, This shit aint scalable though
			Image->SetBrush(Brush);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load texture or image widget is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("File does not exist: %s"), *FilePath);
	}
}

UTexture2D* UUpgradeSelectionWidget::LoadTextureFromFile(const FString& FilePath)
{
	TArray<uint8> RawData;
	if (FFileHelper::LoadFileToArray(RawData, *FilePath))
	{
		// Convert the raw data into a texture (supports PNG, JPG, etc.)
		return FImageUtils::ImportBufferAsTexture2D(RawData);
	}

	return nullptr; // Return null if the image loading failed
}

void UUpgradeSelectionWidget::DisableWidget()
{
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(false);
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		RemoveFromParent();
	}
}


