// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SActionEffect.h"
#include "SActionEffect_Thorn.generated.h"


class USAttributeComponent;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USActionEffect_Thorn : public USActionEffect
{
	GENERATED_BODY()

public:
	USActionEffect_Thorn();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float DamageFractionReflected;

	UFUNCTION()
	void ReflectDamage(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
};
