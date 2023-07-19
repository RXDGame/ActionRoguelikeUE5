// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "SGameModeBase.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global damage multiplier to apply in Attributes"), ECVF_Cheat);

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	MaxHealth = 100;
	CreditsOnDie = 5;
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if(FromActor)
	{
		return FromActor->FindComponentByClass<USAttributeComponent>();
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* FromActor)
{
	USAttributeComponent* AttributeComponent = GetAttributes(FromActor);
	if(AttributeComponent)
	{
		return  AttributeComponent->IsAlive();
	}

	return false;
}

void USAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetMaxHealth());
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if(!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		return false;
	}

	if(Delta < 0.0f)
	{
		const float Multiplier = CVarDamageMultiplier.GetValueOnGameThread();
		Delta *= Multiplier;
	}
	
	const float OldHealth = Health;
	Health = FMath::Clamp(Health + Delta, 0, MaxHealth);

	const float ActualDelta = Health - OldHealth;
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	if(Delta < 0.0f && Health == 0.0f)
	{
		ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if(GM)
		{
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}

	return ActualDelta != 0;
}
