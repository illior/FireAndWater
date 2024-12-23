// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/FAWLevelScriptActor.h"

void AFAWLevelScriptActor::StartTimerForward(FFAWTimeLine& Handle, float Length, bool Rewrite, FTimerDynamicDelegate Event)
{
	Handle.IsReversed = false;
	Handle.FinishTime = Length;

	if (!Event.IsBound())
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	FTimerManager& TimerManager = World->GetTimerManager();
	if (!TimerManager.IsTimerActive(Handle.TimerHandle) || Rewrite)
	{
		Handle.CurrentTime = 0.0f;

		TimerManager.ClearTimer(Handle.TimerHandle);
		TimerManager.SetTimer(Handle.TimerHandle, Event, 0.001f, FTimerManagerTimerParameters{ .bLoop = true, .bMaxOncePerFrame = true, .FirstDelay = 0.001f });
	}
}

void AFAWLevelScriptActor::StartTimerReverse(FFAWTimeLine& Handle, float Length, bool Rewrite, FTimerDynamicDelegate Event)
{
	Handle.IsReversed = true;
	Handle.FinishTime = Length;

	if (!Event.IsBound())
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	FTimerManager& TimerManager = World->GetTimerManager();
	if (!TimerManager.IsTimerActive(Handle.TimerHandle) || Rewrite)
	{
		Handle.CurrentTime = Handle.FinishTime;

		TimerManager.ClearTimer(Handle.TimerHandle);
		TimerManager.SetTimer(Handle.TimerHandle, Event, 0.001f, FTimerManagerTimerParameters{ .bLoop = true, .bMaxOncePerFrame = true, .FirstDelay = 0.001f });
	}
}

float AFAWLevelScriptActor::TickTimer(FFAWTimeLine& Handle)
{
	if (GetWorld() == nullptr)
	{
		return 0.0f;
	}

	if (Handle.IsReversed)
	{
		Handle.CurrentTime = FMath::Max(Handle.CurrentTime - GetWorld()->DeltaTimeSeconds, 0.0f);
	}
	else
	{
		Handle.CurrentTime = FMath::Min(Handle.CurrentTime + GetWorld()->DeltaTimeSeconds, Handle.FinishTime);
	}

	if (Handle.CurrentTime == Handle.FinishTime || Handle.CurrentTime == 0.0f)
	{
		GetWorldTimerManager().ClearTimer(Handle.TimerHandle);
	}

	return Handle.CurrentTime / Handle.FinishTime;
}
