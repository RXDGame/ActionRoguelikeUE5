// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
#include "ActionSystem/SActionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
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
	ActionComponent = CreateDefaultSubobject<USActionComponent>("Action Comp");

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::HandleHealthChanged);
}

void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	for (const TSubclassOf<USAction> ActionClass : DefaultActions)
	{
		ActionComponent->AddAction(ActionClass);
	}
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

void ASCharacter::SprintStart()
{
	ActionComponent->StartActionByName(this, "Sprint");
}

void ASCharacter::SprintStop()
{
	ActionComponent->StopActionByName(this, "Sprint");
}

void ASCharacter::PrimaryAttack()
{
	ActionComponent->StartActionByName(this, "PrimaryAttack");
}

void ASCharacter::SecondaryAttack()
{	
	ActionComponent->StartActionByName(this, "Blackhole");
}

void ASCharacter::Dash()
{
	ActionComponent->StartActionByName(this, "Dash");
}

void ASCharacter::PrimaryInteraction()
{
	InteractionComp->PrimaryInteraction();
}

void ASCharacter::HandleHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if(Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
		if(NewHealth <= 0.0f)
		{
			APlayerController* PC = Cast<APlayerController>(GetController());
			DisableInput(PC);
		}
	}
}

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
	
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::SprintStop);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

void ASCharacter::HealSelf(float Amount /* = 100 */)
{
	AttributeComp->ApplyHealthChange(this, Amount);
}
