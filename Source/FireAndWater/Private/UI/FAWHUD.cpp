// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/FAWHUD.h"

#include "UI/PauseMenu/FAWPauseMenuWidget.h"

void AFAWHUD::OpenPauseMenu()
{
	if (PauseWidget != nullptr)
	{
		PauseWidget->Show();
	}
}

void AFAWHUD::ClosePauseMenu()
{
	if (PauseWidget != nullptr)
	{
		PauseWidget->Hide();
	}
}

void AFAWHUD::BeginPlay()
{
	Super::BeginPlay();

	PauseWidget = CreateWidget<UFAWPauseMenuWidget>(GetOwningPlayerController(), PauseWidgetClass, FName(TEXT("PauseWidget")));
	PauseWidget->AddToViewport();
	PauseWidget->Hide();

	MainWidget = CreateWidget(GetOwningPlayerController(), MainWidgetClass, FName(TEXT("MainWidget")));
	MainWidget->AddToViewport();
}
