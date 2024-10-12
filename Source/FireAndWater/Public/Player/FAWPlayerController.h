// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online.h"
#include "GameFramework/PlayerController.h"
#include "FAWPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class FIREANDWATER_API AFAWPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	AFAWPlayerController();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* MovementMappingContext;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	IOnlineSessionPtr OnlineSession;
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	UFUNCTION(BlueprintCallable)
	void CreateSession();

	UFUNCTION(BlueprintCallable)
	void FindSession();

	void OnSessionCreated(FName InSessionName, bool InWasCreated);
	void OnSessionFounded(bool InWasFounded);
	void OnJoinSession(const FName InSessionName, EOnJoinSessionCompleteResult::Type InResultType);

	virtual void BeginPlay() override;

};
