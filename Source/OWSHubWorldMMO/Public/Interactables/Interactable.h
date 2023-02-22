// Copyright 2023 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/DataTable.h"
#include "Interactable.generated.h"

//Row definition for the Interactables to Spawn Data Table
USTRUCT()
struct FInteractableDataTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FInteractableDataTableRow()
	{
		InteractableGUID = FGuid::NewGuid();
		InteractableTransform = FTransform::Identity;
		bIsPlayerInstanced = false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
		FGuid InteractableGUID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
		FTransform InteractableTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
		TSubclassOf<class AActor> InteractableToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
		bool bIsPlayerInstanced;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OWSHUBWORLDMMO_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void Interact();
	virtual FGuid GetInteractableGUID();
};
