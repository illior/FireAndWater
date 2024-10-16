// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/FAWPlayerController.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Player/FAWBaseCharacter.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AFAWPlayerController::AFAWPlayerController() : 
	OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &AFAWPlayerController::OnSessionCreated)),
	OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &AFAWPlayerController::OnSessionFounded)),
	OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &AFAWPlayerController::OnJoinSession))
{

}

void AFAWPlayerController::CreateSession()
{
	if (!OnlineSession.IsValid())
	{
		return;
	}

	FNamedOnlineSession* NamedSession =  OnlineSession->GetNamedSession(NAME_GameSession);
	if (NamedSession != nullptr)
	{
		OnlineSession->DestroySession(NAME_GameSession);
	}

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	
	FOnlineSessionSettings SessionSetings;
	SessionSetings.bAllowJoinInProgress = true;
	SessionSetings.bIsLANMatch = false;
	SessionSetings.NumPublicConnections = 4;
	SessionSetings.bAllowJoinViaPresence = true;
	SessionSetings.bUsesPresence = true;
	SessionSetings.bShouldAdvertise = true;
	SessionSetings.bUseLobbiesIfAvailable = true;
	SessionSetings.Set(FName(TEXT("illiorPassword")), FString(TEXT("123")), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	OnlineSession->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSetings);
}

void AFAWPlayerController::FindSession()
{
	if (!OnlineSession.IsValid())
	{
		return;
	}

	ULocalPlayer* LocalPlayer = GetLocalPlayer();

	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 100;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	OnlineSession->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, TEXT("Start searching session"));
}

void AFAWPlayerController::OnSessionCreated(FName InSessionName, bool InWasCreated)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, FString::Printf(TEXT("InSessionName = %s, InWasCreated = %s"), *InSessionName.ToString(), InWasCreated ? TEXT("true") : TEXT("false")));

	GetWorld()->ServerTravel(TEXT("/Game/FireAndWater/Maps/TestMap?listen"), true);
}

void AFAWPlayerController::OnSessionFounded(bool InWasFounded)
{
	if (!InWasFounded)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, TEXT("Session not found"));

		return;
	}

	for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
	{
		FString SessionId = Result.GetSessionIdStr();
		FString UserName = Result.Session.OwningUserName;

		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, FString::Printf(TEXT("SessionId = %s, UserName = %s"), *SessionId, *UserName));

		FString Password;
		Result.Session.SessionSettings.Get(FName(TEXT("illiorPassword")), Password);

		if (OnlineSession.IsValid() && Password == FString(TEXT("123")))
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, TEXT("Session found, try connect"));

			ULocalPlayer* LocalPlayer = GetLocalPlayer();

			OnlineSession->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
			break;
		}
	}
}

void AFAWPlayerController::OnJoinSession(const FName InSessionName, EOnJoinSessionCompleteResult::Type InResultType)
{
	switch (InResultType)
	{
	case EOnJoinSessionCompleteResult::Success:
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, TEXT("Join session Success"));
		break;
	case EOnJoinSessionCompleteResult::SessionIsFull:
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, TEXT("Join session SessionIsFull"));
		break;
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, TEXT("Join session SessionDoesNotExist"));
		break;
	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, TEXT("Join session CouldNotRetrieveAddress"));
		break;
	case EOnJoinSessionCompleteResult::AlreadyInSession:
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, TEXT("Join session AlreadyInSession"));
		break;
	case EOnJoinSessionCompleteResult::UnknownError:
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, TEXT("Join session UnknownError"));
		break;
	default:
		break;
	}

	if (InResultType == EOnJoinSessionCompleteResult::Success)
	{
		if (OnlineSession.IsValid())
		{
			FString ConnectionInfo;
			OnlineSession->GetResolvedConnectString(NAME_GameSession, ConnectionInfo);

			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, FString::Printf(TEXT("Connecion info = %s"), *ConnectionInfo));

			ClientTravel(ConnectionInfo, ETravelType::TRAVEL_Absolute);
		}
	}
}

void AFAWPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem != nullptr)
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(MovementMappingContext, 0);
	}

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, FString::Printf(TEXT("Subsystem name = %s"), *OnlineSubsystem->GetSubsystemName().ToString()));

		OnlineSession = OnlineSubsystem->GetSessionInterface();
	}

	if (OnlineSession.IsValid())
	{
		OnlineSession->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
		OnlineSession->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
		OnlineSession->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
}
