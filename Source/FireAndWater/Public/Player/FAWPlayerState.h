// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FAWPlayerState.generated.h"

UCLASS()
class FIREANDWATER_API AFAWPlayerState : public APlayerState
{
	GENERATED_BODY()


public:
	TSubclassOf<APawn> GetPawnClass();

	void AddCheckPoint(AActor* InActor);
	AActor* GetLastCheckPoint() const;

protected:
	UPROPERTY()
	TSubclassOf<APawn> PawnClass;

	UPROPERTY()
	TArray<AActor*> CheckPoints;
};
