// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGamemode.generated.h"

/**
 * 
 */
UCLASS()
class MPMENUSYSTEM_API ALobbyGamemode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* newPlayer) override;
	virtual void Logout(AController* exiting) override;
};
