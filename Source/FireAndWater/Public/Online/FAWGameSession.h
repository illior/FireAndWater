// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GameFramework/GameSession.h"
#include "FAWGameSession.generated.h"

#define MAX_NUM_PLAYER 2;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCreateSessionSignature, FName /* SessionName */, bool /* bWasSuccessful */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDestroySessionSignature, FName /* SessionName */, bool /* bWasSuccessful */);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStartSessionSignature, FName /* SessionName */, bool /* bWasSuccessful */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEndSessionSignature, FName /* SessionName */, bool /* bWasSuccessful */);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFindSessionsSignature, bool /* bWasSuccessful */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnJoinSessionSignature, FName /* SessionName */, EOnJoinSessionCompleteResult::Type /* Result */);

DECLARE_LOG_CATEGORY_EXTERN(LogFAWGameSession, Log, All);

UCLASS()
class FIREANDWATER_API AFAWGameSession : public AGameSession
{
	GENERATED_BODY()


public:
	AFAWGameSession();

	bool CreateSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, bool bIsLAN, bool bShouldAdvertise);
	bool DestroySession(FName InSessionName);

	bool FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, int32 SessionIndexInSearchResults);
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, const FOnlineSessionSearchResult& SearchResult);

	const TArray<FOnlineSessionSearchResult>& GetSearchResults() const;

protected:
	virtual void BeginPlay() override;

	virtual void OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful);
	virtual void OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful);

	virtual void OnStartSessionComplete(FName InSessionName, bool bWasSuccessful);
	virtual void OnEndSessionComplete(FName InSessionName, bool bWasSuccessful);


	virtual void OnFindSessionsComplete(bool bWasSuccessful);
	virtual void OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result);

private:
	TSharedPtr<FOnlineSessionSearch> SearchSettings;
	TSharedPtr<FOnlineSessionSettings> HostSettings;

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FOnEndSessionCompleteDelegate OnEndSessionCompleteDelegate;

	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	FOnCreateSessionSignature OnCreateSessionDelegate;
	FOnDestroySessionSignature OnDestroySessionDelegate;

	FOnStartSessionSignature OnStartSessionDelegate;
	FOnEndSessionSignature OnEndSessionDelegate;

	FOnFindSessionsSignature OnFindSessionsDelegate;
	FOnJoinSessionSignature OnJoinSessionDelegate;

public:
	FOnCreateSessionSignature& OnCreateSession() { return OnCreateSessionDelegate; }
	FOnDestroySessionSignature& OnDestroySessions() { return OnDestroySessionDelegate; }

	FOnStartSessionSignature& OnStartSession() { return OnStartSessionDelegate; }
	FOnEndSessionSignature& OnEndSession() { return OnEndSessionDelegate; }

	FOnFindSessionsSignature& OnFindSessions() { return OnFindSessionsDelegate; }
	FOnJoinSessionSignature& OnJoinSession() { return OnJoinSessionDelegate; }
};
