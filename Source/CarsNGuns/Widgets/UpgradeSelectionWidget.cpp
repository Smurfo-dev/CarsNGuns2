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
		Option1Rarity->SetText(UEnum::GetDisplayValueAsText(Upgrades[0].UpgradeRarity));
		Option1Rarity->SetColorAndOpacity(FSlateColor(RarityColorMap[Upgrades[0].UpgradeRarity]));
	}
	if (Upgrades.Num() > 1)
	{
		SetImageFromFile(Upgrades[1].UpgradeIconFilePath, Option2Image);
		Option2DisplayName->SetText(FText::FromString(Upgrades[1].DisplayName));
		Option2Description->SetText(FText::FromString(Upgrades[1].UpgradeDescription));
		Option2Rarity->SetText(UEnum::GetDisplayValueAsText(Upgrades[1].UpgradeRarity));
		Option2Rarity->SetColorAndOpacity(FSlateColor(RarityColorMap[Upgrades[1].UpgradeRarity]));
	}
	if (Upgrades.Num() > 2)
	{
		SetImageFromFile(Upgrades[2].UpgradeIconFilePath, Option3Image);
		Option3DisplayName->SetText(FText::FromString(Upgrades[2].DisplayName));
		Option3Description->SetText(FText::FromString(Upgrades[2].UpgradeDescription));
		Option3Rarity->SetText(UEnum::GetDisplayValueAsText(Upgrades[2].UpgradeRarity));
		Option3Rarity->SetColorAndOpacity(FSlateColor(RarityColorMap[Upgrades[2].UpgradeRarity]));
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
			UMaterialInterface* Material = Image->GetBrush().GetResourceObject() ? Cast<UMaterialInterface>(Image->GetBrush().GetResourceObject()) : nullptr;
			if (!Material)
			{
				UE_LOG(LogTemp, Error, TEXT("Image does not have a valid material assigned."));
				return;
			}

			UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
			if (!DynMaterial)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to create dynamic material instance."));
				return;
			}
			static FName TextureParamName = TEXT("UpgradeIconTexture"); // must match your material parameter name
			DynMaterial->SetTextureParameterValue(TextureParamName, LoadedTexture);

			FSlateBrush NewBrush;
			NewBrush.SetResourceObject(DynMaterial);
			NewBrush.ImageSize = FVector2D(400, 400);
			Image->SetBrush(NewBrush);
			
			
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


