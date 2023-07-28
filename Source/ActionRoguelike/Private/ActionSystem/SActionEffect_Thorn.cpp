// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/SActionEffect_Thorn.h"
#include "SAttributeComponent.h"
#include "SGameplayFunctionLibrary.h"
#include "ActionSystem/SActionComponent.h"

USActionEffect_Thorn::USActionEffect_Thorn()
{
	Duration = 0.0f;
	Period = 0.0f;
	DamageFractionReflected = 0.1f;
}

void USActionEffect_Thorn::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Instigator);
	if(AttributeComp)
	{
		AttributeComp->OnHealthChanged.AddDynamic(this, &USActionEffect_Thorn::ReflectDamage);
	}
}

void USActionEffect_Thorn::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Instigator);
	if(AttributeComp)
	{
		AttributeComp->OnHealthChanged.RemoveDynamic(this, &USActionEffect_Thorn::ReflectDamage);
	}
}

void USActionEffect_Thorn::ReflectDamage(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if(Delta >= 0)
	{
		return;
	}
	
	if(InstigatorActor == GetOwningComponent()->GetOwner())
	{
		return;
	}

	const int32 ReflectedAmount = FMath::RoundToInt(FMath::Abs(Delta) * DamageFractionReflected);
	USGameplayFunctionLibrary::ApplyDamage(GetOwningComponent()->GetOwner(), InstigatorActor, ReflectedAmount);
}