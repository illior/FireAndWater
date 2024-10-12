// Fill out your copyright notice in the Description page of Project Settings.

#include "FAWGameMode.h"

//#include "OnlineSubsystem.h"

void AFAWGameMode::BeginPlay()
{
	Super::BeginPlay();

	/*IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, FString::Printf(TEXT("Subsystem name = %s"), *OnlineSubsystem->GetSubsystemName().ToString()));

		IOnlineSessionPtr OnlineSession = OnlineSubsystem->GetSessionInterface();
	}*/
}
