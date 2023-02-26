// Copyright 2023 Sabre Dart Studios


#include "DynamicContent/DynamicContentManager.h"
#include "./Interactables/Interactable.h"
#include "../OWSHubWorldMMO.h"

// Sets default values
ADynamicContentManager::ADynamicContentManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADynamicContentManager::BeginPlay()
{
	Super::BeginPlay();

	//Spawn interactables when the map loads on the server
	if (HasAuthority())
	{
		SpawnInteractables();
	}
}

// Called every frame
void ADynamicContentManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADynamicContentManager::SpawnInteractables()
{
	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("ADynamicContentManager - SpawnInteractables Started"));
	
	for (auto InteractableToSpawn : InteractablesToSpawn->GetRowMap())
	{
		//InteractableToSpawn.Key;
		FInteractableDataTableRow* InteractableToSpawnDataTableRow = (FInteractableDataTableRow*)(InteractableToSpawn.Value);
		UE_LOG(OWSHubWorldMMO, Verbose, TEXT("ADynamicContentManager - Attempt to Spawn Interactable: %s"), *InteractableToSpawnDataTableRow->InteractableGUID.ToString());

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		UClass* ClassToSpawn = InteractableToSpawnDataTableRow->InteractableToSpawn.Get();
		AActor* SpawnedActor = GetWorld()->SpawnActor(ClassToSpawn, &InteractableToSpawnDataTableRow->InteractableTransform, SpawnParams);

		if (!SpawnedActor)
		{
			UE_LOG(OWSHubWorldMMO, Error, TEXT("ADynamicContentManager - Unable to Spawn Interactable: %s"), *InteractableToSpawnDataTableRow->InteractableGUID.ToString());
		}

		//Cast to IInteractable to get access to interface methods
		IInteractable* InteractableSupplyPod = Cast<IInteractable>(SpawnedActor);
		if (InteractableSupplyPod)
		{
			InteractableSupplyPod->SetInteractableGUID(InteractableToSpawnDataTableRow->InteractableGUID);
		}
	}
}