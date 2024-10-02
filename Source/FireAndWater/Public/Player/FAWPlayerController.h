// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FAWPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class FIREANDWATER_API AFAWPlayerController : public APlayerController
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* MovementMappingContext;

	virtual void BeginPlay() override;

};
