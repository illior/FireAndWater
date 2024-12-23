// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FAWGameInstance.generated.h"

class AFAWGameSession;

namespace EOnJoinSessionCompleteResult { enum Type; }

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FServerSearchEndDelegate, bool, bWasSuccessful, const TArray<FString>&, ServerNames);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FServerCreateSessionDelegate, FName, InSessionName, bool, bWasSuccessful);

UCLASS()
class FIREANDWATER_API UFAWGameInstance : public UGameInstance
{
	GENERATED_BODY()


public:
	UFAWGameInstance(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	virtual bool CreateSession(ULocalPlayer* LocalPlayer, bool bIsLAN, bool bShouldAdvertise);

	UFUNCTION(BlueprintCallable)
	virtual bool FindSession(ULocalPlayer* LocalPlayer);

	UFUNCTION(BlueprintCallable)
	virtual bool JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults) override;
	virtual bool JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult) override;

	UFUNCTION(BlueprintCallable)
	virtual bool DestroySession();

	AFAWGameSession* GetGameSession() const;
	virtual TSubclassOf<UOnlineSession> GetOnlineSessionClass() override;

	virtual void Init() override;
	virtual void Shutdown() override;

protected:
	UPROPERTY(Config)
	FString MainMenuMap;

	void OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful);

	void OnSearchSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result);

private:
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionsCompleteDelegateHandle;

	FDelegateHandle OnSearchSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

public:
	UPROPERTY(BlueprintAssignable)
	FServerSearchEndDelegate OnServerSearchEnd;

	UPROPERTY(BlueprintAssignable)
	FServerCreateSessionDelegate OnServerCreateSession;
};
