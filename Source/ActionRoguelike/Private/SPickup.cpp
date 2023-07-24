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
	InactiveDuration = 10.0f;
	CreditsCost = 0;

	bReplicates = true;
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
	GetWorldTimerManager().SetTimer(ResetActivation_TimerHandle, this, &ASPickup::ActivatePickup, InactiveDuration);
}
