// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"
#include "SGameplayFunctionLibrary.h"
#include "ActionSystem/SActionComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ActionSystem/SActionEffect.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Comp");
	AudioComponent->SetupAttachment(RootComponent);

	InitialLifeSpan = 10.0f;
	DamageAmount = 20.0f;
}

void ASMagicProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
}

void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	AudioComponent->Play();
}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if(OtherActor && OtherActor != GetInstigator())
	{
		USActionComponent* ActionComp = OtherActor->GetComponentByClass<USActionComponent>();
		if(ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			MovementComp->Velocity *= -1.f;
			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}

		if(USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			Explode();

			if(ActionComp && HasAuthority() && BurningEffectClass != nullptr)
			{
				ActionComp->AddAction(GetInstigator(), BurningEffectClass);
			}
		}
	}
}

void ASMagicProjectile::Explode_Implementation()
{
	Super::Explode_Implementation();

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ImpactSound, GetActorLocation(), FRotator::ZeroRotator,
		1,1, 0, AttenuationSettings);
}

