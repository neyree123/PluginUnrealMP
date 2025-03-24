// Fill out your copyright notice in the Description page of Project Settings.


#include "MPSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

UMPSessionsSubsystem::UMPSessionsSubsystem():
	createSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	findSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	joinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	destroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	startSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();
	if (subsystem)
	{
		sessionInterface = subsystem->GetSessionInterface();
	}
}

void UMPSessionsSubsystem::CreateGameSession(int32 numPublicConnections, FString matchType)
{
	if (!sessionInterface.IsValid())
	{
		return;
	}

	auto existingSession = sessionInterface->GetNamedSession(NAME_GameSession);
	if (existingSession)
	{
		sessionInterface->DestroySession(NAME_GameSession);
	}

	//Add our delegate to the session interface delegate list
	createSessionCompleteDelegateHandle = sessionInterface->AddOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegate);

	//Define our session settings
	lastSessionSettings = MakeShareable(new FOnlineSessionSettings()); //Create a session settings object
	lastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	lastSessionSettings->NumPublicConnections = numPublicConnections;
	lastSessionSettings->bAllowJoinInProgress = true;
	lastSessionSettings->bAllowJoinViaPresence = true;
	lastSessionSettings->bShouldAdvertise = true;
	lastSessionSettings->bUsesPresence = true;
	lastSessionSettings->bUseLobbiesIfAvailable = true;
	lastSessionSettings->Set(FName("matchType"), matchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//Create the new session
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController(); //Get the local player so we can use their id
	if(!sessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *lastSessionSettings))
	{
		//If we could not create a session, remove our delegate from the delegate list
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegateHandle);

		//Broadcast our own custom delegat
		MPOnCreateSessionComplete.Broadcast(false);
	}
}

void UMPSessionsSubsystem::FindSessions(int32 maxSearchResults)
{
	if (!sessionInterface.IsValid())
	{
		return;
	}

	findSessionsCompleteDelegateHandle = sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegate);

	lastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	lastSessionSearch->MaxSearchResults = maxSearchResults;
	lastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	lastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	//Find Sessions
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), lastSessionSearch.ToSharedRef()))
	{
		//If no session found remove our delegate
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegateHandle);
		MPOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMPSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& sessionResult)
{
	if (!sessionInterface.IsValid())
	{
		MPOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	joinSessionCompleteDelegateHandle = sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegate);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, sessionResult))
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegateHandle);
		MPOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMPSessionsSubsystem::DestroySession()
{
}

void UMPSessionsSubsystem::StartSession()
{
}

void UMPSessionsSubsystem::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful)
{
	if (sessionInterface)
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionCompleteDelegateHandle);
	}

	MPOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMPSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (sessionInterface)
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(findSessionsCompleteDelegateHandle);
	}

	if (lastSessionSearch->SearchResults.Num() <= 0)
	{
		MPOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	MPOnFindSessionComplete.Broadcast(lastSessionSearch->SearchResults, bWasSuccessful);
}

void UMPSessionsSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (sessionInterface)
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompleteDelegateHandle);
	}

	MPOnJoinSessionComplete.Broadcast(result);
}

void UMPSessionsSubsystem::OnDestroySessionComplete(FName sessionName, bool bWasSuccessful)
{
}

void UMPSessionsSubsystem::OnStartSessionComplete(FName sessionName, bool bWasSuccessful)
{
}
