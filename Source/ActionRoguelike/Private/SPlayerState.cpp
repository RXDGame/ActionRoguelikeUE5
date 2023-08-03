// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "SSaveGame.h"
#include "Net/UnrealNetwork.h"


ASPlayerState::ASPlayerState()
{
	Credits = 0;
}

void ASPlayerState::AddCredits(int32 Delta)
{
	if(!HasAuthority())
	{
		return;
	}

	Credits += Delta;
	MulticastCreditsChange(Credits, Delta);
}

void ASPlayerState::MulticastCreditsChange_Implementation(int32 NewCredits, int32 Delta)
{
	OnCreditsChange.Broadcast(Delta, NewCredits);
}

bool ASPlayerState::RemoveCredits(int32 Delta)
{
	if(!HasAuthority())
	{
		return false;
	}

	if(Credits < Delta)
	{
		FString FailedMsg = FString::Printf(TEXT("Insufficient credits! Required %i | Current: %i"), Delta, Credits);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);
		return false;
	}

	Credits -= Delta;
	MulticastCreditsChange(Credits, Delta);
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

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
}


