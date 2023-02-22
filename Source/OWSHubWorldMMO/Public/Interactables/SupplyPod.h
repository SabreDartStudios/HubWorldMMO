// Copyright 2023 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "./Interactables/Interactable.h"
#include "SupplyPod.generated.h"

UCLASS()
class OWSHUBWORLDMMO_API ASupplyPod : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASupplyPod();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactable")
		FGuid InteractableGUID;

	//Begin IInteractable Interface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable")
		void Interact();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable")
		FGuid GetInteractableGUID();
	//End IInteractable Interface


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

};
