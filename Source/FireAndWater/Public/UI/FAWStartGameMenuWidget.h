// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/FAWBaseMenuWidget.h"
#include "FAWStartGameMenuWidget.generated.h"

UCLASS()
class FIREANDWATER_API UFAWStartGameMenuWidget : public UFAWBaseMenuWidget
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StartGame")
	FString StartMapUrl;

	virtual void NativeOnInitialized() override;
};
