// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(Abstract)
class ACTIONROGUELIKE_API ASProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASProjectile();

protected:
	
	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	void HandleActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* MovementComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* EffectComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* ImpactVFX;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Explode();
};
