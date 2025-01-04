// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/FAWPlayerController.h"

#include "UI/FAWHUD.h"
#include "GameFramework/GameModeBase.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

AFAWPlayerController::AFAWPlayerController()
{

}

void AFAWPlayerController::ResumeGame()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem != nullptr)
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(MovementMappingContext, 0);
	}

	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;

	AFAWHUD* HUD = GetHUD<AFAWHUD>();
	if (HUD != nullptr)
	{
		HUD->ClosePauseMenu();
	}
}

void AFAWPlayerController::PauseGame()
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem != nullptr)
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(MenuMappingContext, 0);
	}

	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;

	AFAWHUD* HUD = GetHUD<AFAWHUD>();
	if (HUD != nullptr)
	{
		HUD->OpenPauseMenu();
	}
}

void AFAWPlayerController::RestartWithDelay(float Delay)
{
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;

	TimerDelegate.BindLambda([this]() {
		if (GetWorld() != nullptr)
		{
			if (GetWorld()->GetAuthGameMode() != nullptr)
			{
				GetWorld()->GetAuthGameMode()->RestartPlayer(this);
			}
		}
	});

	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, Delay, false);
}

void AFAWPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(PauseGameAction, ETriggerEvent::Started, this, &AFAWPlayerController::InputPause);
	}
}

void AFAWPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem != nullptr)
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(MovementMappingContext, 0);
	}
}

void AFAWPlayerController::InputPause(const FInputActionValue& Value)
{
	PauseGame();
}
