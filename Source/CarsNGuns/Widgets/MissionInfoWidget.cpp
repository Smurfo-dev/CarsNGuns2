// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionInfoWidget.h"

#include "Components/TextBlock.h"

void UMissionInfoWidget::InitializeValues(const FMissionInfo& MissionInfo)
{
	//Expand this when creating real mission info widget prompt thing
	MissionName->SetText(FText::FromString(MissionInfo.MissionName));

	MissionType->SetText(UEnum::GetDisplayValueAsText(MissionInfo.MissionType));
	
	UpgradeType->SetText(UEnum::GetDisplayValueAsText(MissionInfo.UpgradeType));
	
	UpgradeDescription->SetText(FText::FromString(MissionInfo.UpgradeDescription));
	
	PressToStart->SetText(FText::FromString(TEXT("Press 'Enter' To Start")));
}
