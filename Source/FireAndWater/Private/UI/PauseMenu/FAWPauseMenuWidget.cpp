// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PauseMenu/FAWPauseMenuWidget.h"

#include "Player/FAWPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void UFAWPauseMenuWidget::Show()
{
	Super::Show();

	RegisterInputComponent();
}

void UFAWPauseMenuWidget::Hide()
{
	Super::Hide();

	UnregisterInputComponent();
}

void UFAWPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InitializeInputComponent();
	SetupInput(CastChecked<UEnhancedInputComponent>(InputComponent));
}

void UFAWPauseMenuWidget::SetupInput(UEnhancedInputComponent* EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &UFAWPauseMenuWidget::InputMove);

	EnhancedInputComponent->BindAction(ApplyAction, ETriggerEvent::Completed, this, &UFAWPauseMenuWidget::InputApply);
	EnhancedInputComponent->BindAction(CancelAction, ETriggerEvent::Completed, this, &UFAWPauseMenuWidget::InputCancel);
}

void UFAWPauseMenuWidget::MoveWithFrequency(const FVector2D& Value)
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

void UFAWPauseMenuWidget::InputMove(const FInputActionValue& Value)
{
	float CurrentTimeInSeconds = (float)GetWorld()->GetUnpausedTimeSeconds();

	if (LastTimeMovement + MovementFrequency < CurrentTimeInSeconds)
	{
		LastTimeMovement = CurrentTimeInSeconds;

		MoveWithFrequency(Value.Get<FVector2D>());
	}
}

void UFAWPauseMenuWidget::InputApply(const FInputActionValue& Value)
{
	Apply();
}

void UFAWPauseMenuWidget::InputCancel(const FInputActionValue& Value)
{
	if (SubMenu)
	{
		SubMenu->Cancel();
		return;
	}

	AFAWPlayerController* PlayerController = GetOwningPlayer<AFAWPlayerController>();
	if(PlayerController != nullptr)
	{
		PlayerController->ResumeGame();
	}
}
