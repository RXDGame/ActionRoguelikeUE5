// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"
#include "SAttributeComponent.h"
#include "SPlayerState.h"


// Sets default values
ASHealthPotion::ASHealthPotion()
{
	RestoreAmount = 60.0f;
	CreditsCost = 10;
}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if(!Execute_CanInteract(this, InstigatorPawn))
	{
		return;
	}

	ASPlayerState* PS = Cast<ASPlayerState>(InstigatorPawn->GetPlayerState());
	if(PS && !PS->RemoveCredits(CreditsCost))
	{
		return;
	}

	USAttributeComponent* AttributeComponent = InstigatorPawn->GetComponentByClass<USAttributeComponent>();
	if(AttributeComponent)
	{
		if(AttributeComponent->ApplyHealthChange(this, RestoreAmount))
		{
			DeactivatePickup();
		}
	}	
}

bool ASHealthPotion::CanInteract_Implementation(APawn* InstigatorPawn)
{
	if(!Super::CanInteract_Implementation(InstigatorPawn))
	{
		return false;
	}

	const USAttributeComponent* AttributeComponent = InstigatorPawn->GetComponentByClass<USAttributeComponent>();
	if(AttributeComponent)
	{
		return AttributeComponent->GetHealthPercentage() < 1.0f;
	}

	return true;
}

FText ASHealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	const USAttributeComponent* AttributeComponent = InstigatorPawn->GetComponentByClass<USAttributeComponent>();
	if(AttributeComponent && AttributeComponent->IsFullHealth())
	{
		return NSLOCTEXT("InteractableActors", "HealthPotion_FullHealthWarning", "Already at full health.");
	}

	return FText::Format(NSLOCTEXT("InteractableActors", "HealthPotion_InteractMessage", "Cost {0} credits. Restores health to maximum."), CreditsCost);
}

