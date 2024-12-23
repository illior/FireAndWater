// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/FAWMainMenuWidget.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void UFAWMainMenuWidget::QuitGame()
{
	APlayerController* Controller = GetOwningPlayer();
	Controller->ConsoleCommand("quit");
}

void UFAWMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InitializeInputComponent();
	SetupInput(CastChecked<UEnhancedInputComponent>(InputComponent));
}

void UFAWMainMenuWidget::SetupInput(UEnhancedInputComponent* EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &UFAWMainMenuWidget::InputMove);

	EnhancedInputComponent->BindAction(ApplyAction, ETriggerEvent::Completed, this, &UFAWMainMenuWidget::InputApply);
	EnhancedInputComponent->BindAction(CancelAction, ETriggerEvent::Completed, this, &UFAWMainMenuWidget::InputCancel);
}

void UFAWMainMenuWidget::MoveWithFrequency(const FVector2D& Value)
{
	if (Value.Y != 0)
	{
		MoveVertical(-Value.Y);
	}

	if (Value.X != 0)
	{
		MoveHorizontal(Value.X);
	}
}

void UFAWMainMenuWidget::InputMove(const FInputActionValue& Value)
{
	float CurrentTimeInSeconds = (float)GetWorld()->GetUnpausedTimeSeconds();

	if (LastTimeMovement + MovementFrequency < CurrentTimeInSeconds)
	{
		LastTimeMovement = CurrentTimeInSeconds;

		MoveWithFrequency(Value.Get<FVector2D>());
	}
}

void UFAWMainMenuWidget::InputApply(const FInputActionValue& Value)
{
	Apply();
}

void UFAWMainMenuWidget::InputCancel(const FInputActionValue& Value)
{
	if (SubMenu)
	{
		SubMenu->Cancel();
		return;
	}

	QuitGame();
}
