// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"


// Called when the game starts or when spawned
void ASAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(BehaviorTree);
}
