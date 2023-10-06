// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemChest.h"

#include "Net/UnrealNetwork.h"


// Sets default values
ASItemChest::ASItemChest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base Mesh");
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("Lid Mesh");
	LidMesh->SetupAttachment(BaseMesh);

	TargetPitch = 110.0f;
	bReplicates = true;
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	bLidOpened = !bLidOpened;
	OnRep_LidOpened();
}

bool ASItemChest::CanInteract_Implementation(APawn* InstigatorPawn)
{
	return true;
}

FText ASItemChest::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	if(bLidOpened)
	{
		return NSLOCTEXT("InteractableActors", "Chest_Close", "Close chest");
	}

	return NSLOCTEXT("InteractableActors", "Chest_Open", "Open chest");
}

void ASItemChest::OnActorLoaded_Implementation()
{
	OnRep_LidOpened();
}

void ASItemChest::OnRep_LidOpened()
{
	const float CurrentPitch = bLidOpened ? TargetPitch : 0;
	LidMesh->SetRelativeRotation(FRotator(CurrentPitch,0,0));
}

void ASItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASItemChest, bLidOpened);
}
