// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "SAttributeComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Comp");
	AudioComponent->SetupAttachment(RootComponent);

	DamageAmount = -20.0f;
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
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		if(AttributeComp)
		{
			AttributeComp->ApplyHealthChange(GetInstigator(), DamageAmount);
		}

		if(OtherActor->GetClass() != GetClass())
		{
			Explode();
		}
	}
}

void ASMagicProjectile::Explode_Implementation()
{
	Super::Explode_Implementation();
	
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ImpactSound, GetActorLocation(), FRotator::ZeroRotator,
		1,1, 0, AttenuationSettings);
}

