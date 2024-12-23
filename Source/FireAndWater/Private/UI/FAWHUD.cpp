// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/FAWHUD.h"

#include "UI/PauseMenu/FAWPauseMenuWidget.h"

void AFAWHUD::OpenPauseMenu()
{
	if (PauseWidget == nullptr)
	{
		PauseWidget = CreateWidget<UFAWPauseMenuWidget>(GetOwningPlayerController(), PauseWidgetClass, FName(TEXT("PauseWidget")));
	
		PauseWidget->AddToViewport();
	}

	PauseWidget->Show();
}

void AFAWHUD::ClosePauseMenu()
{
	if (PauseWidget == nullptr)
	{
		return;
	}

	PauseWidget->Hide();
}
