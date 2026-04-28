// Copyright 2023 Sabre Dart Studios

#include "Game/HWPlayerState.h"
#include "Player/HWPlayerController.h"
#include "Misc/Base64.h"
#include "../OWSHubWorldMMO.h"

AHWPlayerState::AHWPlayerState()
{
	InventoryComponent = CreateDefaultSubobject<URiftInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);

	// Must be set before BeginPlay so URiftInventoryComponent::InitializeInventory()
	// finds a backend and kicks off the async load.
	InventoryComponent->SetPersistenceObject(this);

	bLoadInventoryRequested = false;
}

// -- IRiftPersistenceInterface --

void AHWPlayerState::SaveInventory_Implementation(const FRiftInventorySaveData& Data, const FOnSaveComplete& OnComplete)
{
	if (!HasAuthority())
	{
		// Inventory mutations are server-authoritative; clients must never call OWS directly.
		UE_LOG(OWSHubWorldMMO, Warning, TEXT("AHWPlayerState::SaveInventory - Called without authority, skipped."));
		OnComplete.ExecuteIfBound(false);
		return;
	}

	AHWPlayerController* PC = Cast<AHWPlayerController>(GetOwningController());
	if (!PC || !PC->OWSPlayerControllerComponent)
	{
		UE_LOG(OWSHubWorldMMO, Warning, TEXT("AHWPlayerState::SaveInventory - No valid PlayerController, inventory save skipped."));
		OnComplete.ExecuteIfBound(false);
		return;
	}

	FString Base64String = FBase64::Encode(Data.InventoryData);
	PC->OWSPlayerControllerComponent->AddOrUpdateCustomCharacterData(
		GetPlayerName(), TEXT("RiftInventory"), Base64String);

	// Fire optimistically — the OWS call is async but we don't block the game on it.
	OnComplete.ExecuteIfBound(true);
}

void AHWPlayerState::LoadInventory_Implementation(const FString& InPlayerId, const FOnLoadComplete& OnComplete)
{
	if (!HasAuthority())
	{
		// Client inventory state arrives via bIsInitialized replication, not the persistence path.
		return;
	}

	// Mark that InitializeInventory has run and containers are ready.
	// DeliverInventoryLoadResult uses this to know it can safely populate the component.
	bLoadInventoryRequested = true;

	// If DeliverInventoryLoadResult already fired before we were ready, deliver now.
	if (bHasCachedResult)
	{
		bHasCachedResult = false;
		DeliverInventoryLoadResult(bCachedSuccess, CachedInventoryData);
	}
}

void AHWPlayerState::DeliverInventoryLoadResult(bool bSuccess, const FString& Base64String)
{
	if (!HasAuthority())
	{
		// Client inventory state arrives via bIsInitialized replication, not the persistence path.
		return;
	}
	/*
	if (!bLoadInventoryRequested)
	{
		// InitializeInventory hasn't run yet — cache until LoadInventory_Implementation fires.
		bHasCachedResult = true;
		bCachedSuccess = bSuccess;
		CachedInventoryData = Base64String;
		return;
	}
	*/
	FRiftInventorySaveData SaveData;
	SaveData.PlayerId = GetPlayerName();

	if (bSuccess && !Base64String.IsEmpty())
	{
		FBase64::Decode(Base64String, SaveData.InventoryData);
	}

	InventoryComponent->DeliverLoadResult(bSuccess, SaveData);
}
