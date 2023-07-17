// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"
#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"


USBTTask_RangedAttack::USBTTask_RangedAttack()
{
	MaxBulletSpread = 2.0f;
}

EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* MyController = OwnerComp.GetAIOwner();
	if(!ensure(MyController))
	{
		return EBTNodeResult::Failed;
	}

	ACharacter* MyCharacter = Cast<ACharacter>(MyController->GetPawn());
	if(!ensure(MyCharacter))
	{
		return EBTNodeResult::Failed;
	}

	const FVector MuzzleLocation = MyCharacter->GetMesh()->GetSocketLocation("Muzzle_01");
	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if(TargetActor == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if(!USAttributeComponent::IsActorAlive(TargetActor))
	{
		return EBTNodeResult::Failed;
	}

	const FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;

	FRotator MuzzleRotation = Direction.Rotation();
	MuzzleRotation.Pitch += FMath::RandRange(0.0f, MaxBulletSpread);
	MuzzleRotation.Yaw += FMath::RandRange(-MaxBulletSpread, MaxBulletSpread);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Instigator = MyCharacter;

	const AActor* NewProj = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParameters);
	return NewProj ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
