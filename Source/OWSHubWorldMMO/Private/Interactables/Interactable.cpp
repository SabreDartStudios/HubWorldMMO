// Copyright 2023 Sabre Dart Studios


#include "Interactables/Interactable.h"
#include "../OWSHubWorldMMO.h"

// Add default functionality here for any IInteractable functions that are not pure virtual.

void IInteractable::Interact()
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("IInteractable - Interact Started"));
}

FGuid IInteractable::GetInteractableGUID()
{
	return FGuid::NewGuid();
}

void IInteractable::SetInteractableGUID(FGuid NewInteractableGUID)
{

}