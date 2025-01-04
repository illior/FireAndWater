// Fill out your copyright notice in the Description page of Project Settings.

#include "FAWGameMode.h"

#include "Player/FAWPlayerState.h"

void AFAWGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}

AActor* AFAWGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	AFAWPlayerState* PlayerState = Player->GetPlayerState<AFAWPlayerState>();
	if (PlayerState == nullptr || PlayerState->GetLastCheckPoint() == nullptr)
	{
		return Super::FindPlayerStart_Implementation(Player, IncomingName);
	}

	return PlayerState->GetLastCheckPoint();
}

UClass* AFAWGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	AFAWPlayerState* PlayerState = InController->GetPlayerState<AFAWPlayerState>();
	if (PlayerState != nullptr)
	{
		return PlayerState->GetPawnClass();
	}

	return DefaultPawnClass;
}

void AFAWGameMode::BeginPlay()
{
	Super::BeginPlay();
}
