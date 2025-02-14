// Fill out your copyright notice in the Description page of Project Settings.

#include "FAWGameMode.h"

#include "Player/FAWPlayerState.h"
#include "Components/FAWHealthComponent.h"

void AFAWGameMode::StartBossFight(AActor* InActor)
{
	if (InActor == nullptr)
	{
		return;
	}

	UFAWHealthComponent* BossHealth = InActor->GetComponentByClass<UFAWHealthComponent>();
	if (BossHealth == nullptr)
	{
		return;
	}

	if (BossHealth->IsDead())
	{
		return;
	}

	BossHealth->OnDeath.AddDynamic(this, &AFAWGameMode::EndBossFight);

	BossActor = InActor;
	SetCurrentState(EFAWGameState::Fighting);
}

void AFAWGameMode::EndBossFight()
{
	if (BossActor != nullptr)
	{
		UFAWHealthComponent* BossHealth = BossActor->GetComponentByClass<UFAWHealthComponent>();
		if (BossHealth != nullptr)
		{
			BossHealth->OnDeath.RemoveDynamic(this, &AFAWGameMode::EndBossFight);
		}
	}

	BossActor = nullptr;
	SetCurrentState(EFAWGameState::Exploring);
}

void AFAWGameMode::SetCurrentState(EFAWGameState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;
	MatchStateChanged.Broadcast(CurrentState);
}

EFAWGameState AFAWGameMode::GetCurrentState()
{
	return CurrentState;
}

AActor* AFAWGameMode::GetBossActor()
{
	return BossActor;
}

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
