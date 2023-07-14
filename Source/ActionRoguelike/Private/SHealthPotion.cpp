// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"
#include "SAttributeComponent.h"


// Sets default values
ASHealthPotion::ASHealthPotion()
{
	RestoreAmount = 60.0f;
}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if(!CanInteract_Implementation(InstigatorPawn))
	{
		return;
	}

	USAttributeComponent* AttributeComponent = InstigatorPawn->GetComponentByClass<USAttributeComponent>();
	if(AttributeComponent)
	{
		AttributeComponent->ApplyHealthChange(RestoreAmount);
		DeactivatePickup();
	}	
}

bool ASHealthPotion::CanInteract_Implementation(APawn* InstigatorPawn)
{
	if(bInactive)
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

