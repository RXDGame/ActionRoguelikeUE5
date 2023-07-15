// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if(!ensure(Blackboard))
	{
		return;
	}

	const AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject("TargetActor"));
	if(!TargetActor)
	{
		return;
	}

	const AAIController* MyController = OwnerComp.GetAIOwner();
	if(!ensure(MyController))
	{
		return;
	}

	const APawn* AIPawn = MyController->GetPawn();
	if(ensure(AIPawn))
	{
		const float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());
		const bool bWithinRange = DistanceTo < 2000.0f;

		bool bHasLoS = false;
		if(bWithinRange)
		{
			bHasLoS = MyController->LineOfSightTo(TargetActor);
		}
		
		Blackboard->SetValueAsBool(AttackRangeKey.SelectedKeyName, bWithinRange && bHasLoS);
	}
}
