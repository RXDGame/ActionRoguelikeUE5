// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionSystem/SAction.h"
#include "SAction_ProjectileAttack.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()

public:
	USAction_ProjectileAttack();

	virtual void StartAction_Implementation(AActor* Instigator) override;

	UFUNCTION()
	void AttackDelay_Elapsed(ACharacter* Character);

	FVector GetAimHit(ACharacter* Character) const;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackAnimDelay;
	
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName HandSocketName;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UParticleSystem> CastingSpellVFX;
};
