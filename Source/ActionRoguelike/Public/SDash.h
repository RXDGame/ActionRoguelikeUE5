﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <SProjectile.h>
#include "SDash.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASDash : public ASProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASDash();

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly)
	float DetonateDelay;

	UPROPERTY(EditDefaultsOnly)
	float TeleportDelay;
	
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void Explode_Implementation() override;
	void Teleport();


	FTimerHandle BeforeExplode_TimerHandle;
	FTimerHandle AfterExplode_TimerHandle;
};
