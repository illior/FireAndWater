// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FAWGameMode.generated.h"

UENUM(BlueprintType)
enum class EFAWGameState : uint8
{
	Exploring,
	Fighting
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMatchStateChangedSignature, EFAWGameState, NewState);

UCLASS()
class FIREANDWATER_API AFAWGameMode : public AGameMode
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintAssignable)
	FMatchStateChangedSignature MatchStateChanged;

	UClass* GetDefaultPawnClass() const { return DefaultPawnClass; };
	UClass* GetDefaultSecondPawnClass() const { return DefaultSecondPawnClass; };

	UFUNCTION(BlueprintCallable)
	void StartBossFight(AActor* InActor);

	UFUNCTION(BlueprintCallable)
	void EndBossFight();

	void SetCurrentState(EFAWGameState NewState);
	EFAWGameState GetCurrentState();

	AActor* GetBossActor();

	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = TEXT("")) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes, meta = (DisplayAfter = "DefaultPawnClass"))
	TSubclassOf<APawn> DefaultSecondPawnClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> BossActor;

	UPROPERTY(BlueprintReadOnly)
	EFAWGameState CurrentState = EFAWGameState::Exploring;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	virtual void BeginPlay() override;
};
