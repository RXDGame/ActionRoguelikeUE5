// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickup.h"
#include "Components/SphereComponent.h"


// Sets default values
ASPickup::ASPickup()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>("Sphere Comp");
	RootComponent = SphereComponent;

	MeshComponent  =CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	MeshComponent->SetupAttachment(SphereComponent);

	bInactive = false;
	RespawnDelay = 10.0f;
	CreditsCost = 0;

	bReplicates = true;
}

bool ASPickup::CanInteract_Implementation(APawn* InstigatorPawn)
{
	return !bInactive;
}

void ASPickup::Interact_Implementation(APawn* InstigatorPawn)
{
	if(Execute_CanInteract(this, InstigatorPawn))
	{
		DeactivatePickup();
	}
}

void ASPickup::ActivatePickup()
{
	bInactive = false;
	MeshComponent->SetVisibility(true);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ASPickup::DeactivatePickup()
{
	bInactive = true;
	MeshComponent->SetVisibility(false);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if(RespawnDelay > 0.0f)
	{
		GetWorldTimerManager().SetTimer(ResetActivation_TimerHandle, this, &ASPickup::ActivatePickup, RespawnDelay);
	}
}
