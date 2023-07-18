// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/SAction.h"

#include "ActionSystem/SActionComponent.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), * GetNameSafe(this));
	
	GetOwningComponent()->ActiveGameplayTags.AppendTags(GrantsTags);
	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopping: %s"), * GetNameSafe(this));

	ensureAlways(bIsRunning);
	
	GetOwningComponent()->ActiveGameplayTags.RemoveTags(GrantsTags);
	bIsRunning = false;
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
	return bIsRunning;
}
