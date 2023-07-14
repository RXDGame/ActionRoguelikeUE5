// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SPickup.generated.h"

class USphereComponent;

UCLASS(Abstract)
class ACTIONROGUELIKE_API ASPickup : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:
	ASPickup();
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(BlueprintReadOnly)
	bool bInactive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InactiveDuration;
	
	FTimerHandle ResetActivation_TimerHandle;
	
	void ActivatePickup();
	void DeactivatePickup();
};
