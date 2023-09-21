// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

class USMonsterData;
class USSaveGame;

USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

	FMonsterInfoRow()
	{
		Weight = 1.0f;
		SpawnCost = 5.0f;
		KillReward = 20.0f;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USMonsterData* MonsterData {};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
};

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	FString SlotName;
	
	UPROPERTY()
	USSaveGame* CurrentSaveGame; 

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnInterval;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UDataTable* MonsterData;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;
	
	FTimerHandle TimerHandle_SpawnBots;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

public:
	ASGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void StartPlay() override;

	virtual void OnActorKilled(AActor* VictimActor, AActor* KillerActor);

	UFUNCTION(Exec)
	void KillAll();

protected:
	UFUNCTION()
	void OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);
	
	void SpawnBotTimerElapsed();

	bool CanSpawnBot() const;

public:

	UFUNCTION(BlueprintCallable, Category = "Save Game")
	void WriteSaveGame();

	void LoadSaveGame();
};
