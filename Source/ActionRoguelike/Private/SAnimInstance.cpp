// Fill out your copyright notice in the Description page of Project Settings.


#include "SAnimInstance.h"

#include "GameplayTagContainer.h"
#include "ActionSystem/SActionComponent.h"

void USAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	const AActor* OwningActor = GetOwningActor();
	if(OwningActor)
	{
		ActionComponent = OwningActor->GetComponentByClass<USActionComponent>();
	}
}

void USAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const FGameplayTag StunnedTag = FGameplayTag::RequestGameplayTag("Status.Stunned");
	if(ActionComponent)
	{
		bIsStunned = ActionComponent->ActiveGameplayTags.HasTag(StunnedTag);
	}
}
