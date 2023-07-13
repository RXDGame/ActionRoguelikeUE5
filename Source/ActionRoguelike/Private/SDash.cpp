// Fill out your copyright notice in the Description page of Project Settings.


#include "SDash.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ASDash::ASDash()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ExplosionEffect = CreateDefaultSubobject<UParticleSystemComponent>("Explosion Effect");
	ExplosionEffect->SetupAttachment(RootComponent);
	ExplosionEffect->SetAutoActivate(false);
}

void ASDash::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SphereComp->OnComponentHit.AddDynamic(this, &ASDash::HandleHit);
}

// Called when the game starts or when spawned
void ASDash::BeginPlay()
{
	Super::BeginPlay();
	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
	GetWorldTimerManager().SetTimer(BeforeExplode_TimerHandle, this, &ASDash::Explode, 0.2f);
}

void ASDash::HandleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor == GetInstigator())
	{
		return;
	}
	
	GetWorldTimerManager().ClearTimer(BeforeExplode_TimerHandle);
	Explode();
}

void ASDash::Explode()
{
	MovementComp->StopMovementImmediately();
	SetActorEnableCollision(false);
	EffectComp->Deactivate();
	ExplosionEffect->Activate();
	
	GetWorldTimerManager().SetTimer(BeforeExplode_TimerHandle, this, &ASDash::Teleport, 0.2f);
}

void ASDash::Teleport()
{
	APawn* InstigatorPawn = GetInstigator();
	if(ensure(InstigatorPawn))
	{
		InstigatorPawn->TeleportTo(GetActorLocation(), InstigatorPawn->GetActorRotation(), false, false);
	}
}
