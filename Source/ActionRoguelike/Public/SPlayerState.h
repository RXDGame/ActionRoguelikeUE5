// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCreditsChangeDelegate, int, Delta, int, NewCredits);

UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASPlayerState();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Credits")
	int Credits;

public:

	UPROPERTY(BlueprintAssignable)
	FCreditsChangeDelegate OnCreditsChange;

	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool ApplyCreditChange(int Amount);

	UFUNCTION(BlueprintCallable, Category = "Credits")
	int GetCredits() const;
};
