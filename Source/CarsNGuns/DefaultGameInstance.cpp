// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"

void UDefaultGameInstance::Init()
{
	Super::Init();
	
}

void UDefaultGameInstance::StartTimer()
{
	if (bIsPaused)
	{
		bIsPaused = false;
		StartTime = GetWorld()->GetTimeSeconds();
		PausedElapsedTime = 0.0f;
	}
}

void UDefaultGameInstance::PauseTimer()
{
	if (!bIsPaused)
	{
		PausedElapsedTime = GetElapsedTime();
		bIsPaused = true;
	}
}

void UDefaultGameInstance::ResumeTimer()
{
	if (bIsPaused)
	{
		bIsPaused = false;
		StartTime = GetWorld()->GetTimeSeconds() - PausedElapsedTime;
	}
}

void UDefaultGameInstance::StopTimer()
{
	bIsPaused = true;
}

float UDefaultGameInstance::GetElapsedTime() const
{
	if (!bIsPaused)
	{
		return GetWorld()->GetTimeSeconds() - StartTime;
	}
	return PausedElapsedTime;
}

