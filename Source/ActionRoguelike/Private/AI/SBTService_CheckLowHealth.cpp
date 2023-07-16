// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckLowHealth.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckLowHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* MyController = OwnerComp.GetAIOwner();
	if(!MyController)
	{
		return;
	}

	const APawn* Pawn = MyController->GetPawn();
	const USAttributeComponent* AttributeComponent = Pawn->GetComponentByClass<USAttributeComponent>();	
	if(AttributeComponent)
	{
		UBlackboardComponent* Blackboard = MyController->GetBlackboardComponent();
		if(ensureMsgf(Blackboard, TEXT("AI controller doesn't have a Behavior Tree assigned. Please assign a Behaviour Tree")))
		{
			Blackboard->SetValueAsBool(LowHealthKey.SelectedKeyName, AttributeComponent->GetHealthPercentage() <= LowHealthRatioThreshold);
		}
	}
}
