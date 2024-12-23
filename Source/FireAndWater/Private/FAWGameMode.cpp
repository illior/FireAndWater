// Fill out your copyright notice in the Description page of Project Settings.

#include "FAWGameMode.h"

UClass* AFAWGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	int32 Players = GetNumPlayers();
	if (Players % 2 == 1)
	{
		return DefaultPawnClass;
	}
	else
	{
		return DefaultSecondPawnClass;
	}
}

void AFAWGameMode::BeginPlay()
{
	Super::BeginPlay();
}
