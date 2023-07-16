// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_Heal.h"

#include "AIController.h"
#include "SAttributeComponent.h"

EBTNodeResult::Type USBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	if(!Controller)
	{
		return EBTNodeResult::Failed;
	}

	const APawn* Pawn = Controller->GetPawn();
	USAttributeComponent* AttributeComponent = Pawn->GetComponentByClass<USAttributeComponent>();
	if(!AttributeComponent)
	{
		return EBTNodeResult::Failed;
	}

	AttributeComponent->ApplyHealthChange(AttributeComponent->GetMaxHealth());
	return EBTNodeResult::Succeeded;
}
