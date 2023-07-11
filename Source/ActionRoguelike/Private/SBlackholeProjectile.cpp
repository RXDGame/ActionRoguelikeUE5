// Fill out your copyright notice in the Description page of Project Settings.


#include "SBlackholeProjectile.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
ASBlackholeProjectile::ASBlackholeProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ForceComp = CreateDefaultSubobject<URadialForceComponent>("Force Component");
	ForceComp->SetupAttachment(SphereComp);
	ForceComp->Radius = 700.0f;
	ForceComp->ForceStrength = -2000.0f;	
}

