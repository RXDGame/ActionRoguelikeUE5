// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystem/SAction_ProjectileAttack.h"

#include "SCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	AttackAnimDelay = 0.2f;
	HandSocketName = "Muzzle_01";
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);
	if(Character)
	{
		Character->PlayAnimMontage(AttackAnim);
		UGameplayStatics::SpawnEmitterAttached(CastingSpellVFX, Character->GetMesh(), "Muzzle_01",
			FVector::ZeroVector, FRotator::ZeroRotator,EAttachLocation::SnapToTarget);

		FTimerHandle TimerHandle_AttackDelay;
		FTimerDelegate TimerDelegate_Attack;
		TimerDelegate_Attack.BindUFunction(this, "AttackDelay_Elapsed", Character);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, TimerDelegate_Attack, AttackAnimDelay, false);
	}
}

void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* Character)
{
	if(ensureAlways(ProjectileClass))
	{
		const FVector HandLocation = Character->GetMesh()->GetSocketLocation("Muzzle_01");
		const FVector EndLocation = GetAimHit(Character);
		const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, EndLocation);
	
		const FTransform SpawnTM = FTransform(TargetRotation, HandLocation);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = Character;
	
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}

	StopAction(Character);
}

FVector USAction_ProjectileAttack::GetAimHit(ACharacter* Character) const
{
	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

	FVector Start = Character->GetPawnViewLocation();
	FVector Direction = Character->GetControlRotation().Vector();	
	FVector EndLocation = Start + Direction * 5000.0f;

	FCollisionShape Shape;
	Shape.SetSphere(15.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	FHitResult HitResult;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	if(GetWorld()->SweepSingleByObjectType(HitResult, Start, EndLocation, FQuat::Identity, ObjectQueryParams, Shape, QueryParams))
	{
		EndLocation = HitResult.Location;
	}

	return EndLocation;
}
