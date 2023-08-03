// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"
#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global damage multiplier to apply in Attributes"), ECVF_Cheat);

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	MaxHealth = 100;
	Health = MaxHealth;
	CreditsOnDie = 5;

	MaxRage = 100;
	Rage = 0;
	RageMultiplierFromDamage = 3.f;

	SetIsReplicatedByDefault(true);
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
	const float LocalHealth = FMath::Clamp(Health + Delta, 0, MaxHealth);
	const float ActualDelta = LocalHealth - OldHealth;

	// is server?
	if(GetOwner()->HasAuthority())
	{
		Health = LocalHealth;
		if(ActualDelta != 0)
		{
			MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
		}
		
		if(ActualDelta < 0.0f)
		{
			if(Health == 0.0f)
			{
				ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
				if(GM)
				{
					GM->OnActorKilled(GetOwner(), InstigatorActor);
				}
			}
			else
			{
				ApplyRageChange(InstigatorActor, FMath::Abs(ActualDelta) * RageMultiplierFromDamage);
			}		
		}
	}	

	return ActualDelta != 0;
}

bool USAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	if(Rage + Delta < 0.0f)
	{
		return false;
	}
	
	const float OldRage = Rage;
	const float LocalRage = FMath::Clamp(Rage + Delta, 0.0f, MaxRage);
	const float RageDelta = LocalRage - OldRage;

	if(GetOwner()->HasAuthority())
	{
		Rage = LocalRage;
		if(RageDelta != 0)
		{
			MulticastRageChanged(InstigatorActor, Rage, RageDelta);
		}
	}

	return RageDelta != 0;
}

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* Instigator, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(Instigator,this, NewHealth, Delta);
}

void USAttributeComponent::MulticastRageChanged_Implementation(AActor* Instigator, float NewRage, float Delta)
{
	OnRageChanged.Broadcast(Instigator, this, NewRage, Delta);
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, MaxHealth);
}