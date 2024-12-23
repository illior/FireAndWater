// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FAWCoreTypes.generated.h"

USTRUCT(BlueprintType)
struct FFAWTimeLine
{
	GENERATED_BODY()


public:
	FTimerHandle TimerHandle;

	bool IsReversed = false;
	float CurrentTime = 0.0f;
	float FinishTime = 1.0f;
};
