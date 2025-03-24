// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGamemode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGamemode::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);

	if (GameState)
	{
		int32 numOfPlayers = GameState.Get()->PlayerArray.Num();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				60.f,
				FColor::Yellow,
				FString::Printf(TEXT("Players in game: %d"), numOfPlayers)
				);

			APlayerState* playerState = newPlayer->GetPlayerState<APlayerState>();
			if (playerState)
			{
				FString playerName = playerState->GetPlayerName();

				GEngine->AddOnScreenDebugMessage(
					-1,
					60.f,
					FColor::Cyan,
					FString::Printf(TEXT("%s has joined the game!"), *playerName)
				);
			}
		}
	}
}

void ALobbyGamemode::Logout(AController* exiting)
{
	Super::Logout(exiting);

	if (GameState)
	{
		int32 numOfPlayers = GameState.Get()->PlayerArray.Num();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				60.f,
				FColor::Yellow,
				FString::Printf(TEXT("Players in game: %d"), numOfPlayers - 1)
			);

			APlayerState* playerState = exiting->GetPlayerState<APlayerState>();
			if (playerState)
			{
				FString playerName = playerState->GetPlayerName();

				GEngine->AddOnScreenDebugMessage(
					-1,
					60.f,
					FColor::Cyan,
					FString::Printf(TEXT("%s has exited the game!"), *playerName)
				);
			}
		}
	}
}
