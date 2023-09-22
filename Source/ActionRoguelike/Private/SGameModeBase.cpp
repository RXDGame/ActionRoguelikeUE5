// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SGameplayInterface.h"
#include "SMonsterData.h"
#include "SPlayerState.h"
#include "SSaveGame.h"
#include "ActionRoguelike/ActionRoguelike.h"
#include "ActionSystem/SActionComponent.h"
#include "AI/SAICharacter.h"
#include "Engine/AssetManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnInterval = 2.0f;
	SlotName = "SaveGame01";
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	LoadSaveGame();
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if(PS)
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnInterval, true);
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* KillerActor)
{
	ASCharacter* Player = Cast<ASCharacter>(VictimActor);
	if(Player)
	{
		FTimerHandle TimerHandle_RespawnDelay;

		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

		constexpr float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}
	else
	{
		int Credits = 1;
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(VictimActor);
		if(AttributeComp)
		{
			Credits = AttributeComp->CreditsOnDie;
		}

		APawn* KillerPawn = Cast<APawn>(KillerActor);
		if(KillerPawn)
		{
			if(ASPlayerState* PlayerState = KillerPawn->GetPlayerState<ASPlayerState>())
			{				
				PlayerState->AddCredits(Credits);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("OnActorKilled - Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(KillerActor));
}


void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if(ensure(Controller))
	{
		Controller->UnPossess();		// if not unposses, still grab the same character and only reset rotation
		RestartPlayer(Controller);
	}
}

void ASGameModeBase::KillAll()
{	
	for(TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;		
		USAttributeComponent* AttributeComponent = USAttributeComponent::GetAttributes(Bot);
		if(ensure(AttributeComponent) && AttributeComponent->IsAlive())
		{
			AttributeComponent->Kill(this); // change to player
		}
	}
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	if(!CanSpawnBot())
	{
		return;
	}
	
	const auto QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	if(ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);
	}
}

bool ASGameModeBase::CanSpawnBot() const
{
	if(!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot via timer is disabled via Cvar"));
		return false;
	}
	
	int32 NumberOfAliveBots = 0;
	for(TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;		
		USAttributeComponent* AttributeComponent = USAttributeComponent::GetAttributes(Bot);
		if(ensure(AttributeComponent) && AttributeComponent->IsAlive())
		{
			NumberOfAliveBots++;
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("Number of Bots alive: %i"), NumberOfAliveBots);

	float MaxNumberOfBot = 10.0f;
	if(DifficultyCurve)
	{
		MaxNumberOfBot = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}
	
	if(NumberOfAliveBots >= MaxNumberOfBot)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't spawn. Max number of bots reached."));
		return false;
	}

	return true;
}
void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
                                      EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("EQS Bot Spawning failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if(Locations.Num() > 0)
	{
		if(MonsterTable)
		{
			TArray<FMonsterInfoRow*> Rows;
			MonsterTable->GetAllRows("", Rows);

			const int32 Index = FMath::RandRange(0, Rows.Num() - 1);
			const FMonsterInfoRow* SelectedRow = Rows[Index];

			UAssetManager* AssetManager = UAssetManager::GetIfValid();
			if(AssetManager)
			{
				TArray<FName> Bundle;
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this,
					&ASGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, Locations[0]);

				LogOnScreen(this, "Loading asset...", FColor::Green);
				AssetManager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundle, Delegate);
			}
		}
	}
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedAssetId, FVector SpawnLocation)
{
	LogOnScreen(this, "Asset loaded!", FColor::Green);
	const UAssetManager* AssetManager = UAssetManager::GetIfValid();
	if(AssetManager)
	{
		USMonsterData* MonsterData =  Cast<USMonsterData>(AssetManager->GetPrimaryAssetObject(LoadedAssetId));
		if(!MonsterData)
		{
			return;
		}
	
		if(AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator))
		{
			USActionComponent* ActionComp = NewBot->GetComponentByClass<USActionComponent>();
			if(!ActionComp)
			{
				return;
			}

			for (const TSubclassOf<USAction> ActionClass : MonsterData->Actions)
			{
				ActionComp->AddAction(NewBot, ActionClass);
			}
		}
	}
}

void ASGameModeBase::WriteSaveGame()
{
	for(int32 i=0; i < GameState->PlayerArray.Num(); i++)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if(PS)
		{
			PS->SavePlayerState(CurrentSaveGame);
			break; // only for single player at the moment
		}
	}

	CurrentSaveGame->SavedActors.Empty();
	for(FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if(!Actor->Implements<USGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.Transform = Actor->GetTransform();

		FMemoryWriter MemoryWriter(ActorData.ByteData);
		FObjectAndNameAsStringProxyArchive Arc(MemoryWriter, true);
		Arc.ArIsSaveGame = true; // only consider variables with UPROPERTY(SaveGame)

		Actor->Serialize(Arc);
		
		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}

void ASGameModeBase::LoadSaveGame()
{
	if(UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if(CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame data."));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame data."));

		for(FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			if(!Actor->Implements<USGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if(ActorData.ActorName == Actor->GetName())
				{
					Actor->SetActorTransform(ActorData.Transform);

					FMemoryReader MemoryReader(ActorData.ByteData);
					FObjectAndNameAsStringProxyArchive Arc(MemoryReader, true);
					Arc.ArIsSaveGame = true; // only consider variables with UPROPERTY(SaveGame)

					Actor->Serialize(Arc);
					ISGameplayInterface::Execute_OnActorLoaded(Actor);
					break;
				}
			}
		}
	}
	else
	{
		CurrentSaveGame =  Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
		UE_LOG(LogTemp, Log, TEXT("Created new SaveGame data."));
	}
}
