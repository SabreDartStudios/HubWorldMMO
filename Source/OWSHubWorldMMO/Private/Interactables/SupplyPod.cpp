// Copyright 2023 Sabre Dart Studios


#include "Interactables/SupplyPod.h"

// Sets default values
ASupplyPod::ASupplyPod()
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ASupplyPod::BeginPlay()
{
	Super::BeginPlay();
	
}


void ASupplyPod::Interact_Implementation()
{

}

FGuid ASupplyPod::GetInteractableGUID_Implementation()
{
	return InteractableGUID;
}