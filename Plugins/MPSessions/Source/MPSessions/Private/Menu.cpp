// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MPSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(int32 _numPublicConnections, FString _matchType, FString _lobbyPath)
{
	numPublicConnections = _numPublicConnections;
	matchType = _matchType;
	pathToLobby = FString::Printf(TEXT("%s?listen"), *_lobbyPath);
	
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeUIOnly inputModeData;
			inputModeData.SetWidgetToFocus(TakeWidget());
			inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			playerController->SetInputMode(inputModeData);
			playerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* gameInstance = GetGameInstance();
	if (gameInstance)
	{
		MPSessionSubsystem = gameInstance->GetSubsystem<UMPSessionsSubsystem>();
	}

	//Bind our custom callbacks
	if (MPSessionSubsystem)
	{
		MPSessionSubsystem->MPOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MPSessionSubsystem->MPOnFindSessionComplete.AddUObject(this, &ThisClass::OnFindSession);
		MPSessionSubsystem->MPOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MPSessionSubsystem->MPOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MPSessionSubsystem->MPOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMenu::HostButtonClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMenu::JoinButtonClicked);
	}

	return true;
}

void UMenu::NativeDestruct()
{
	MenuTeardown();
	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Green,
				FString(TEXT("Session Created Sucessfully"))
			);
		}

		UWorld* world = GetWorld();
		if (world)
		{
			world->ServerTravel(pathToLobby);
		}
	}
	else 
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Red,
				FString(TEXT("Failed to Create Session"))
			);
		}

		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& sessionResults, bool bWasSuccessful)
{
	if (MPSessionSubsystem == nullptr)
	{
		return;
	}

	for (auto result : sessionResults)
	{
		FString id = result.GetSessionIdStr();
		FString user = result.Session.OwningUserName;

		FString settingsValue;
		result.Session.SessionSettings.Get(FName("MatchType"), settingsValue);
		if (settingsValue == matchType)
		{
			MPSessionSubsystem->JoinSession(result);
			return;
		}
	}

	if (!bWasSuccessful || sessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type result)
{
	IOnlineSubsystem* subsystem = IOnlineSubsystem::Get();
	if (subsystem)
	{
		IOnlineSessionPtr sessionInterface = subsystem->GetSessionInterface();
		if (sessionInterface.IsValid())
		{
			return;
		}

		FString address;
		if (sessionInterface->GetResolvedConnectString(NAME_GameSession, address))
		{
			APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (playerController)
			{
				playerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
			}
		}
	}

	if (result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (MPSessionSubsystem)
	{
		MPSessionSubsystem->CreateGameSession(numPublicConnections, matchType);
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (MPSessionSubsystem)
	{
		MPSessionSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTeardown()
{
	RemoveFromParent();
	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeGameOnly inputModeData;
			playerController->SetInputMode(inputModeData);
			playerController->SetShowMouseCursor(false);
		}
	}
}
