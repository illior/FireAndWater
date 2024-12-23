// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FAWCoreTypes.h"
#include "Engine/LevelScriptActor.h"
#include "FAWLevelScriptActor.generated.h"

UCLASS()
class FIREANDWATER_API AFAWLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()


public:

protected:
	UFUNCTION(BlueprintCallable)
	void StartTimerForward(UPARAM(ref) FFAWTimeLine& Handle, float Length, bool Rewrite, FTimerDynamicDelegate Event);

	UFUNCTION(BlueprintCallable)
	void StartTimerReverse(UPARAM(ref) FFAWTimeLine& Handle, float Length, bool Rewrite, FTimerDynamicDelegate Event);

	UFUNCTION(BlueprintCallable)
	float TickTimer(UPARAM(ref) FFAWTimeLine& Handle);
};
