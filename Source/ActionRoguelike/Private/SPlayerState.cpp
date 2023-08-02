// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "SSaveGame.h"


ASPlayerState::ASPlayerState()
{
	Credits = 0;
}

void ASPlayerState::AddCredits(int32 Delta)
{
	Credits += Delta;
	OnCreditsChange.Broadcast(Delta, Credits);
}

bool ASPlayerState::RemoveCredits(int32 Delta)
{
	if(Credits < Delta)
	{
		FString FailedMsg = FString::Printf(TEXT("Insufficient credits! Required %i | Current: %i"), Delta, Credits);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);
		return false;
	}

	Credits -= Delta;
	OnCreditsChange.Broadcast(Delta, Credits);
	return true;
}

int ASPlayerState::GetCredits() const
{
	return Credits;
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if(SaveObject)
	{
		SaveObject->Credits = Credits;
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if(SaveObject)
	{
		Credits = SaveObject->Credits;
	}
}

