// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewValue, float, Delta);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USAttributeComponent();

	UFUNCTION(BlueprintCallable)
	static USAttributeComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable)
	static bool IsActorAlive(AActor* FromActor);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	float MaxHealth;
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Atrributes")
	float Health;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	float MaxRage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated)
	float RageMultiplierFromDamage;
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Atrributes")
	float Rage;

	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable) //@FIXME: mark me as unreliable once we moved the "state" of our scharacter
	void MulticastHealthChanged(AActor* Instigator, float NewHealth, float Delta);
	
	UFUNCTION(NetMulticast, Reliable) //@FIXME: mark me as unreliable once we moved the "state" of our scharacter
	void MulticastRageChanged(AActor* Instigator, float NewRage, float Delta);

public:	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Credits")
	int CreditsOnDie;

	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnRageChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyRageChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable)
	float GetHealthPercentage() const {return Health / MaxHealth;}

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const {return MaxHealth;}

	UFUNCTION(BlueprintCallable)
	bool IsFullHealth() const;
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxRage() const { return MaxRage; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentRage() const { return Rage; }
};
