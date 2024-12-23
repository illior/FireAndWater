// Fill out your copyright notice in the Description page of Project Settings.

#include "Online/FAWGameSession.h"
#include "Online.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"

DEFINE_LOG_CATEGORY(LogFAWGameSession);

AFAWGameSession::AFAWGameSession() :
	OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &AFAWGameSession::OnCreateSessionComplete)),
	OnDestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &AFAWGameSession::OnDestroySessionComplete)),
	OnStartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &AFAWGameSession::OnStartSessionComplete)),
	OnEndSessionCompleteDelegate(FOnEndSessionCompleteDelegate::CreateUObject(this, &AFAWGameSession::OnEndSessionComplete)),
	OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &AFAWGameSession::OnFindSessionsComplete)),
	OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &AFAWGameSession::OnJoinSessionComplete))
{

}

bool AFAWGameSession::CreateSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, bool bIsLAN, bool bShouldAdvertise)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem == nullptr)
	{
		return false;
	}

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (SessionInterface.IsValid() && UserId.IsValid())
	{
		HostSettings = MakeShareable(new FOnlineSessionSettings());
		HostSettings->bAllowInvites = true;
		HostSettings->bAllowJoinInProgress = true;
		HostSettings->bAllowJoinViaPresence = true;
		HostSettings->bUseLobbiesIfAvailable = true;
		HostSettings->bUsesPresence = true;

		HostSettings->bIsLANMatch = bIsLAN;
		HostSettings->bShouldAdvertise = bShouldAdvertise;
		HostSettings->NumPublicConnections = MAX_NUM_PLAYER;
		//HostSettings->Set(FName(TEXT("Gameillior")), true, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		return SessionInterface->CreateSession(*UserId, InSessionName, *HostSettings);
	}

	return false;
}

bool AFAWGameSession::DestroySession(FName InSessionName)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem == nullptr)
	{
		return false;
	}

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(NAME_GameSession);
		if (NamedSession != nullptr)
		{
			return SessionInterface->DestroySession(NAME_GameSession);
		}

		return true;
	}

	return false;
}

bool AFAWGameSession::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem == nullptr)
	{
		return false;
	}

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (SessionInterface.IsValid() && UserId.IsValid())
	{
		SearchSettings = MakeShareable(new FOnlineSessionSearch());
		SearchSettings->bIsLanQuery = bIsLAN;
		SearchSettings->MaxSearchResults = 50;

		if (bIsPresence)
		{
			SearchSettings->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
		}
	
		return SessionInterface->FindSessions(*UserId, SearchSettings.ToSharedRef());
	}

	return false;
}

bool AFAWGameSession::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, int32 SessionIndexInSearchResults)
{
	if (SearchSettings->SearchResults.IsValidIndex(SessionIndexInSearchResults))
	{
		return JoinSession(UserId, InSessionName, SearchSettings->SearchResults[SessionIndexInSearchResults]);
	}

	return false;
}

bool AFAWGameSession::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName InSessionName, const FOnlineSessionSearchResult& SearchResult)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem == nullptr)
	{
		return false;
	}

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (SessionInterface.IsValid() && UserId.IsValid())
	{
		return SessionInterface->JoinSession(*UserId, InSessionName, SearchResult);
	}

	return false;
}

const TArray<FOnlineSessionSearchResult>& AFAWGameSession::GetSearchResults() const
{
	return SearchSettings->SearchResults;
}

void AFAWGameSession::BeginPlay()
{
	Super::BeginPlay();

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem != nullptr)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
			SessionInterface->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
			SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
			SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
			SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
		}
	}
}

void AFAWGameSession::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	UE_LOG(LogFAWGameSession, Display, TEXT("OnCreateSessionComplete %s bSuccess: %d"), *InSessionName.ToString(), bWasSuccessful);

	OnCreateSessionDelegate.Broadcast(InSessionName, bWasSuccessful);
}

void AFAWGameSession::OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful)
{
	UE_LOG(LogFAWGameSession, Display, TEXT("OnDestroySessionComplete %s bSuccess: %d"), *InSessionName.ToString(), bWasSuccessful);

	OnDestroySessionDelegate.Broadcast(InSessionName, bWasSuccessful);
}

void AFAWGameSession::OnStartSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	UE_LOG(LogFAWGameSession, Display, TEXT("OnStartSessionComplete %s bSuccess: %d"), *InSessionName.ToString(), bWasSuccessful);

	OnStartSessionDelegate.Broadcast(InSessionName, bWasSuccessful);
}

void AFAWGameSession::OnEndSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	UE_LOG(LogFAWGameSession, Display, TEXT("OnEndSessionComplete %s bSuccess: %d"), *InSessionName.ToString(), bWasSuccessful);

	OnEndSessionDelegate.Broadcast(InSessionName, bWasSuccessful);
}

void AFAWGameSession::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogFAWGameSession, Display, TEXT("OnFindSessionsComplete %d"), bWasSuccessful);

	IOnlineSubsystem* const OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem == nullptr)
	{
		return;
	}

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		return;
	}

	UE_LOG(LogFAWGameSession, Display, TEXT("Num Search Results: %d"), SearchSettings->SearchResults.Num());
	if (SearchSettings->SearchResults.Num() > 0)
	{
		for (int32 SearchId = 0; SearchId < SearchSettings->SearchResults.Num(); SearchId++)
		{
			DumpSession(&SearchSettings->SearchResults[SearchId].Session);
		}
	}

	OnFindSessionsDelegate.Broadcast(bWasSuccessful);
}

void AFAWGameSession::OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogFAWGameSession, Display, TEXT("OnJoinSessionComplete %s bSuccess: %d"), *InSessionName.ToString(), static_cast<int32>(Result));

	OnJoinSessionDelegate.Broadcast(InSessionName, Result);
}
