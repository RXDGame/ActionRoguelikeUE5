// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/SAction.h"
#include "ActionSystem/SActionComponent.h"
#include "Net/UnrealNetwork.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), * GetNameSafe(this));
	//LogOnScreen(GetWorld(), FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);
	
	GetOwningComponent()->ActiveGameplayTags.AppendTags(GrantsTags);
	RepData.bIsRunning = true;
	RepData.Instigator = Instigator;

	if(GetOwningComponent()->GetOwnerRole() == ROLE_Authority)
	{
		TimeStarted = GetWorld()->TimeSeconds;
	}

	GetOwningComponent()->OnActionStarted.Broadcast(GetOwningComponent(), this);
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopping: %s"), * GetNameSafe(this));
	//LogOnScreen(GetWorld(), FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::White);

	//ensureAlways(bIsRunning);
	
	GetOwningComponent()->ActiveGameplayTags.RemoveTags(GrantsTags);
	RepData.bIsRunning = false;
	RepData.Instigator = Instigator;
	
	GetOwningComponent()->OnActionStopped.Broadcast(GetOwningComponent(), this);
}

void USAction::OnRep_RepData()
{
	if (RepData.bIsRunning)
	{
		StartAction(RepData.Instigator);
	}
	else
	{
		StopAction(RepData.Instigator);
	}
}

bool USAction::CanStart(AActor* Instigator)
{
	if(IsRunning())
	{
		return false;
	}
	
	if(GetOwningComponent()->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}

	return true;
}

UWorld* USAction::GetWorld() const
{
	// Outer is set when creating via NewObject<>
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if(Comp)
	{
		return Comp->GetWorld();
	}

	return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const
{
	return Cast<USActionComponent>(GetOuter());
}

bool USAction::IsRunning() const
{
	return RepData.bIsRunning;
}

void USAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAction, RepData);
	DOREPLIFETIME(USAction, TimeStarted);
}

