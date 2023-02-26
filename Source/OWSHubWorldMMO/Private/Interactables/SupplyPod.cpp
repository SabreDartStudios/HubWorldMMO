// Copyright 2023 Sabre Dart Studios


#include "Interactables/SupplyPod.h"
#include "Net/UnrealNetwork.h"
#include "Player/HWPlayerController.h"


// Sets default values
ASupplyPod::ASupplyPod()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ASupplyPod::BeginPlay()
{
	Super::BeginPlay();
	
	//Only run this on the client
	if (!HasAuthority())
	{
		//The first time this is spawned the Guid will be invalid (all zero's). Skip the initialization and wait for the Guid to be set in SetInteractableGUID_Implementation
		if (!InteractableGUID.IsValid())
		{
			return;
		}

		//Supply pod has been opened
		if (IsSupplyPodOpened())
		{
			SupplyPodOpened();
		}
		else //Supply pod is still closed
		{
			SupplyPodClosed();
		}
	}
}

bool ASupplyPod::IsSupplyPodOpened()
{
	//Get the player controller
	AHWPlayerController* HWPlayerController = Cast<AHWPlayerController>(GetWorld()->GetFirstPlayerController());
	if (HWPlayerController)
	{
		//Supply pod has been opened
		if (HWPlayerController->IsSupplyPodOpened(InteractableGUID))
		{
			return true;
		}
	}

	return false;
}

void ASupplyPod::SupplyPodOpened_Implementation()
{
	//No default implementation.  We handle this in BP by overriding SupplyPodOpened();
}

void ASupplyPod::SupplyPodClosed_Implementation()
{
	//No default implementation.  We handle this in BP by overriding SupplyPodClosed();
}


void ASupplyPod::Interact_Implementation()
{
	
}

FGuid ASupplyPod::GetInteractableGUID_Implementation()
{
	return InteractableGUID;
}

void ASupplyPod::SetInteractableGUID_Implementation(FGuid NewInteractableGUID)
{
	InteractableGUID = NewInteractableGUID;
}

//Called on all clients when the InteractableGUID is set on the Server
void ASupplyPod::OnRep_InteractableGUID()
{
	//Supply pod has been opened
	if (IsSupplyPodOpened())
	{
		SupplyPodOpened();
	}
	else //Supply pod is still closed
	{
		SupplyPodClosed();
	}
}

void ASupplyPod::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Only replicate to Owner
	DOREPLIFETIME(ASupplyPod, InteractableGUID);
}