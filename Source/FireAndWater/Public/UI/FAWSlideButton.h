// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FAWSlideButton.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UFAWSlideButton : public UInterface
{
	GENERATED_BODY()
};

class FIREANDWATER_API IFAWSlideButton
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Button")
	void MoveHorizontal(int32 Value);
	virtual void MoveHorizontal_Implementation(int32 Value);
};
