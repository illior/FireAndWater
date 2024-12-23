// Fill out your copyright notice in the Description page of Project Settings.

#include "FAWGameInstance.h"

#include "Online.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/GameMode.h"
#include "Online/FAWOnlineSession.h"
#include "Online/FAWGameSession.h"

#include "Interfaces/OnlineSessionInterface.h"

UFAWGameInstance::UFAWGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UFAWGameInstance::CreateSession(ULocalPlayer* LocalPlayer, bool bIsLAN, bool bShouldAdvertise)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Black, FString::Printf(TEXT("Session bIsLAN: %i, bShouldAdvertise: %i"), bIsLAN, bShouldAdvertise));

	AFAWGameSession* GameSession = GetGameSession();
	if (GameSession != nullptr)
	{
		GameSession->OnCreateSession().RemoveAll(this);
		OnCreateSessionCompleteDelegateHandle = GameSession->OnCreateSession().AddUObject(this, &UFAWGameInstance::OnCreateSessionComplete);

		return GameSession->CreateSession(LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), NAME_GameSession, bIsLAN, bShouldAdvertise);
	}

	return false;
}

bool UFAWGameInstance::FindSession(ULocalPlayer* LocalPlayer)
{
	AFAWGameSession* GameSession = GetGameSession();
	if (GameSession != nullptr)
	{
		GameSession->OnFindSessions().RemoveAll(this);
		OnSearchSessionsCompleteDelegateHandle = GameSession->OnFindSessions().AddUObject(this, &UFAWGameInstance::OnSearchSessionComplete);

		return GameSession->FindSessions(LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), false, true);
	}

	return false;
}

AFAWGameSession* UFAWGameInstance::GetGameSession() const
{
	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		AGameModeBase* const Game = World->GetAuthGameMode();
		if (Game != nullptr)
		{
			return Cast<AFAWGameSession>(Game->GameSession);
		}
	}

	return nullptr;
}

TSubclassOf<UOnlineSession> UFAWGameInstance::GetOnlineSessionClass()
{
	return UFAWOnlineSession::StaticClass();
}

bool UFAWGameInstance::JoinSession(ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults)
{
	AFAWGameSession* GameSession = GetGameSession();
	if (GameSession != nullptr)
	{
		GameSession->OnJoinSession().RemoveAll(this);
		OnJoinSessionCompleteDelegateHandle = GameSession->OnJoinSession().AddUObject(this, &UFAWGameInstance::OnJoinSessionComplete);

		return GameSession->JoinSession(LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), NAME_GameSession, SessionIndexInSearchResults);
	}

	return false;
}

bool UFAWGameInstance::JoinSession(ULocalPlayer* LocalPlayer, const FOnlineSessionSearchResult& SearchResult)
{
	AFAWGameSession* GameSession = GetGameSession();
	if (GameSession != nullptr)
	{
		GameSession->OnJoinSession().RemoveAll(this);
		OnJoinSessionCompleteDelegateHandle = GameSession->OnJoinSession().AddUObject(this, &UFAWGameInstance::OnJoinSessionComplete);

		return GameSession->JoinSession(LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId(), NAME_GameSession, SearchResult);
	}

	return false;
}

bool UFAWGameInstance::DestroySession()
{
	AFAWGameSession* GameSession = GetGameSession();
	if (GameSession != nullptr)
	{
		GameSession->OnDestroySessions().RemoveAll(this);
		OnCreateSessionCompleteDelegateHandle = GameSession->OnDestroySessions().AddUObject(this, &UFAWGameInstance::OnDestroySessionComplete);

		return GameSession->DestroySession(NAME_GameSession);
	}
	else
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
	}

	return false;
}

void UFAWGameInstance::Init()
{
	Super::Init();

}

void UFAWGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UFAWGameInstance::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	OnServerCreateSession.Broadcast(InSessionName, bWasSuccessful);

	AFAWGameSession* GameSession = GetGameSession();
	if (GameSession != nullptr)
	{
		GameSession->OnCreateSession().Remove(OnCreateSessionCompleteDelegateHandle);
	}

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem == nullptr)
	{
		return;
	}

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* Session = SessionInterface->GetNamedSession(InSessionName);
	
		if (Session != nullptr)
		{
			FString Str;

			switch (Session->SessionState)
			{
			case EOnlineSessionState::NoSession:
				Str = FString("NoSession");
				break;
			case EOnlineSessionState::Creating:
				Str = FString("Creating");
				break;
			case EOnlineSessionState::Pending:
				Str = FString("Pending");
				break;
			case EOnlineSessionState::Starting:
				Str = FString("Starting");
				break;
			case EOnlineSessionState::InProgress:
				Str = FString("InProgress");
				break;
			case EOnlineSessionState::Ending:
				Str = FString("Ending");
				break;
			case EOnlineSessionState::Ended:
				Str = FString("Ended");
				break;
			case EOnlineSessionState::Destroying:
				Str = FString("Destroying");
				break;
			default:
				break;
			}

			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, FString::Printf(TEXT("Session State %s"), *Str));
		}
	}
}

void UFAWGameInstance::OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful)
{
	AFAWGameSession* GameSession = GetGameSession();
	if (GameSession != nullptr)
	{
		GameSession->OnCreateSession().Remove(OnCreateSessionCompleteDelegateHandle);
	}
}

void UFAWGameInstance::OnSearchSessionComplete(bool bWasSuccessful)
{
	TArray<FString> ServerNames;

	AFAWGameSession* GameSession = GetGameSession();
	if (GameSession != nullptr)
	{
		GameSession->OnFindSessions().Remove(OnSearchSessionsCompleteDelegateHandle);

		const TArray<FOnlineSessionSearchResult>& SearchResults = GameSession->GetSearchResults();
		if (SearchResults.Num() > 0)
		{
			for (int32 i = 0; i < SearchResults.Num(); i++)
			{
				ServerNames.Add(SearchResults[i].Session.OwningUserName);
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, FString::Printf(TEXT("Session name: %s"), *SearchResults[i].Session.OwningUserName));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, FString::Printf(TEXT("No Session found")));
		}
	}

	OnServerSearchEnd.Broadcast(bWasSuccessful, ServerNames);
}

void UFAWGameInstance::OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result)
{
	AFAWGameSession* GameSession = GetGameSession();
	if (GameSession != nullptr)
	{
		GameSession->OnJoinSession().Remove(OnJoinSessionCompleteDelegateHandle);
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		return;
	}

	if (LocalPlayers.Num() == 1)
	{
		APlayerController* const PlayerController = GetFirstLocalPlayerController();

		FString URL;

		IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
		if (OnlineSubsystem == nullptr)
		{
			return;
		}
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (!SessionInterface.IsValid())
		{
			return;
		}

		SessionInterface->GetResolvedConnectString(InSessionName, URL);
		PlayerController->ClientTravel(URL, TRAVEL_Absolute);
	}
}
