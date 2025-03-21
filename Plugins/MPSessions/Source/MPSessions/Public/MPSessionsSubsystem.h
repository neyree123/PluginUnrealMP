// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MPSessionsSubsystem.generated.h"

//
// Declaring our own custom delegates for the Menu class to use
//
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMPOnCreateSessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class MPSESSIONS_API UMPSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMPSessionsSubsystem();

	//
	// To handle session functionality. The menu class will call these.
	//
	void CreateGameSession(int32 numPublincConnections, FString matchType);
	void FindSessions(int32 maxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& sessionResult);
	void DestroySession();
	void StartSession();

	//
	//Our custom delegates for the Menu class
	//
	FMPOnCreateSessionComplete MPOnCreateSessionComplete;

protected:

	//
	// Internal callbacks to be hooked up to our session delegates
	//

	void OnCreateSessionComplete(FName sessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	void OnDestroySessionComplete(FName sessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName sessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr sessionInterface;
	TSharedPtr<FOnlineSessionSettings> lastSessionSettings;

	//
	// To add to the Online Session Interface delegate lists
	// We will bind our internal callbacks to these
	//

	FOnCreateSessionCompleteDelegate createSessionCompleteDelegate;
	FDelegateHandle createSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate findSessionsCompleteDelegate;
	FDelegateHandle findSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate joinSessionCompleteDelegate;
	FDelegateHandle joinSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate destroySessionCompleteDelegate;
	FDelegateHandle destroySessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate startSessionCompleteDelegate;
	FDelegateHandle startSessionCompleteDelegateHandle;

};
