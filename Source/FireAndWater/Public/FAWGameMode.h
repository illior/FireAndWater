// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FAWGameMode.generated.h"

UCLASS()
class FIREANDWATER_API AFAWGameMode : public AGameMode
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes, meta = (DisplayAfter = "DefaultPawnClass"))
	TSubclassOf<APawn> DefaultSecondPawnClass;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	virtual void BeginPlay() override;
};
