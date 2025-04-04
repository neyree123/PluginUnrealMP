// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MPSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 _numOfPublicConnections = 4, FString _matchType = FString(TEXT("FreeForAll")), FString _lobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/NewLobby")));

protected: 

	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//
	// Callback for the custom delegates on the MPSessionsSubsystem
	//
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& sessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTeardown();

	class UMPSessionsSubsystem* MPSessionSubsystem;

	int32 numPublicConnections{ 4 };
	FString matchType{ TEXT("FreeForAll") };
	FString pathToLobby{ TEXT("") };
};
