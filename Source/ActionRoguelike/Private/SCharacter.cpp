// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("Camera Comp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("Interaction Comp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("Attribute Comp");

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0;
	ControlRot.Roll= 0;

	AddMovementInput(ControlRot.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0;
	ControlRot.Roll= 0;

	const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector, Value);	
}

void ASCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_FireProjectile, 0.2f);	
}

void ASCharacter::SecondaryAttack()
{	
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_SecondaryAttack, this, &ASCharacter::SecondaryAttack_FireProjectile, 0.2f);
}

void ASCharacter::Dash()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &ASCharacter::FireDashProjectile, 0.2f);	
}

void ASCharacter::PrimaryAttack_FireProjectile()
{
	const FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	const FVector EndLocation = GetAimHit();
	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, EndLocation);
	
	SpawnProjectile(PrimaryProjectileClass, HandLocation, TargetRotation);
}

void ASCharacter::SecondaryAttack_FireProjectile()
{
	const FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	const FVector EndLocation = GetAimHit();
	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, EndLocation);
	
	SpawnProjectile(BlackholeProjectileClass, HandLocation, TargetRotation);
}

void ASCharacter::FireDashProjectile()
{
	const FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	const FVector EndLocation = GetAimHit();
	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, EndLocation);
	
	SpawnProjectile(DashProjectileClass, HandLocation, TargetRotation);
}

void ASCharacter::SpawnProjectile(TSubclassOf<AActor> ProjectileClass, FVector InLocation, FRotator InRotation)
{
	if(!ensure(ProjectileClass))
	{
		return;
	}

	const FTransform SpawnTM = FTransform(InRotation, InLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;
	
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
}

void ASCharacter::PrimaryInteraction()
{
	InteractionComp->PrimaryInteraction();
}

FVector ASCharacter::GetAimHit()
{
	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());

	FVector WorldLocation, WorldDirection;
	if(!PlayerController->DeprojectScreenPositionToWorld(ViewportSize.X / 2, ViewportSize.Y / 2, WorldLocation, WorldDirection))
	{
		return FVector::Zero();
	}
	
	FVector EndLocation = WorldLocation + WorldDirection * 5000.0f;	

	FHitResult Hit;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	if(GetWorld()->LineTraceSingleByObjectType(Hit, WorldLocation, EndLocation, ObjectQueryParams))
	{
		return Hit.ImpactPoint;
	}

	return EndLocation;
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::SecondaryAttack);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASCharacter::Dash);
	
	PlayerInputComponent->BindAction("PrimaryInteraction", IE_Pressed, this, &ASCharacter::PrimaryInteraction);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::StopJumping);
}