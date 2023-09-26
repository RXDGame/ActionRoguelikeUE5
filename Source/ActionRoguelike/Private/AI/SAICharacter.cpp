// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "SAttributeComponent.h"
#include "SWorldUserWidget.h"
#include "ActionSystem/SActionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("Pawn Sensing Comp");
	AttributeComponent = CreateDefaultSubobject<USAttributeComponent>("Attribute Comp");
	ActionComponent = CreateDefaultSubobject<USActionComponent>("Action Comp");

	TargetActorKeyName = "TargetActor";
	TimeToHitParam = "TimeToHit";
	bReplicates = true;

	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	AttributeComponent->OnHealthChanged.AddDynamic(this, &ASAICharacter::HandleHealthChanged);
}

void ASAICharacter::HandleHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if(Delta < 0.0f)
	{
		if(InstigatorActor != this)
		{
			SetTargetActor(InstigatorActor);
		}

		if(ActiveHealthBar == nullptr)
		{
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport();
			}
		}
		
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParam, GetWorld()->TimeSeconds);
		if(NewHealth <= 0.0f)
		{
			Die();
		}
	}
}


void ASAICharacter::SetTargetActor(AActor* NewActor)
{
	AAIController* AIControler = Cast<AAIController>(GetController());
	if(AIControler)
	{
		AIControler->GetBlackboardComponent()->SetValueAsObject(TargetActorKeyName, NewActor);
	}

	if(NewActor != CurrentTarget)
	{
		CurrentTarget = NewActor;
		AddSpottedUI();
	}
}

void ASAICharacter::AddSpottedUI()
{
	if(ActiveSpotted != nullptr)
	{
		ActiveSpotted->RemoveFromParent();
	}
			
	ActiveSpotted = CreateWidget<USWorldUserWidget>(GetWorld(), SpottedWidgetClass);
	ActiveSpotted->AttachedActor = this;
	ActiveSpotted->AddToViewport();
}

void ASAICharacter::Die()
{
	// stop behavior tree
	AAIController* AIC = Cast<AAIController>(GetController());
	if(AIC)
	{
		AIC->GetBrainComponent()->StopLogic("Killed");
	}

	// ragdoll
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName("Ragdoll");

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	// Destroy
	SetLifeSpan(10.0f);
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	MulticastPawnSeen(Pawn);
}

void ASAICharacter::MulticastPawnSeen_Implementation(APawn* Pawn)
{
	SetTargetActor(Pawn);
}

void ASAICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASAICharacter,CurrentTarget);
	DOREPLIFETIME(ASAICharacter,ActiveSpotted);	
}

