// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"

#include "SAttributeComponent.h"
#include "SPlayerState.h"
#include "Kismet/GameplayStatics.h"

bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	USAttributeComponent* AttributeComponent = USAttributeComponent::GetAttributes(TargetActor);
	if(AttributeComponent)
	{
		return AttributeComponent->ApplyHealthChange(DamageCauser, -DamageAmount);
	}

	return false;
}

bool USGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount,
	const FHitResult& Hit)
{
	if(ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		UPrimitiveComponent* HitComponent = Hit.GetComponent();
		if(HitComponent && HitComponent->IsSimulatingPhysics(Hit.BoneName))
		{
			FVector Direction = Hit.TraceEnd - Hit.TraceStart;
			Direction.Normalize();
			HitComponent->AddImpulseAtLocation(Direction * 300000.f, Hit.Location, Hit.BoneName);
		}
		return true;
	}

	return false;
}

bool USGameplayFunctionLibrary::ApplyCreditsChange(UWorld* WorldContext, int PlayerIndex, int CreditsToChange)
{
	ASPlayerState* PlayerState = Cast<ASPlayerState>(UGameplayStatics::GetPlayerState(WorldContext, PlayerIndex));
	if(PlayerState)
	{
		return PlayerState->ApplyCreditChange(CreditsToChange);
	}

	return false;
}
