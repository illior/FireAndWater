// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/FAWPlayerState.h"

#include "FAWGameMode.h"

TSubclassOf<APawn> AFAWPlayerState::GetPawnClass()
{
	if (PawnClass != nullptr)
	{
		return PawnClass;
	}

	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	AFAWGameMode* GameMode = World->GetAuthGameMode<AFAWGameMode>();
	if (GameMode == nullptr)
	{
		return nullptr;
	}

	int32 Players = GameMode->GetNumPlayers();
	PawnClass = Players % 2 == 0 ? GameMode->GetDefaultPawnClass() : GameMode->GetDefaultSecondPawnClass();

	return PawnClass;
}

void AFAWPlayerState::AddCheckPoint(AActor* InActor)
{
	if (InActor != nullptr && !CheckPoints.Contains(InActor))
	{
		CheckPoints.Add(InActor);
	}
}

AActor* AFAWPlayerState::GetLastCheckPoint() const
{
	if (CheckPoints.Num() > 0)
	{
		return CheckPoints[CheckPoints.Num() - 1];
	}

	return nullptr;
}
