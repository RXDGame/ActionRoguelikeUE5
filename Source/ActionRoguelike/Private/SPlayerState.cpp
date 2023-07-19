// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"


ASPlayerState::ASPlayerState()
{
	Credits = 0;
}

bool ASPlayerState::ApplyCreditChange(int Amount)
{
	const int NewCredits = Credits + Amount;
	if(Amount < 0 && NewCredits < 0)
	{
		FString FailedMsg = FString::Printf(TEXT("Insufficient credits! Required %i | Current: %i"), Amount, Credits);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);
		return false;
	}

	Credits = NewCredits;
	OnCreditsChange.Broadcast(Amount, Credits);
	return true;
}

int ASPlayerState::GetCredits() const
{
	return Credits;
}

