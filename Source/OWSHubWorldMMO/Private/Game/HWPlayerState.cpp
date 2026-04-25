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
		// Intentionally do not store or fire the callback on clients.
		return;
	}

	// Store the callback. The actual data arrives later when
	// AHWPlayerController::NotifyGetCustomCharacterData dispatches the "RiftInventory" row.
	PendingLoadDelegate = OnComplete;

	// If DeliverInventoryLoadResult already fired before BeginPlay stored our delegate, fire now.
	if (bHasCachedResult)
	{
		bHasCachedResult = false;
		DeliverInventoryLoadResult(bCachedSuccess, CachedInventoryData);
	}
}

void AHWPlayerState::DeliverInventoryLoadResult(bool bSuccess, const FString& Base64String)
{
	if (!PendingLoadDelegate.IsBound())
	{
		// LoadInventory_Implementation hasn't run yet — cache the result so it fires immediately when it does.
		bHasCachedResult = true;
		bCachedSuccess = bSuccess;
		CachedInventoryData = Base64String;
		return;
	}

	FRiftInventorySaveData SaveData;
	SaveData.PlayerId = GetPlayerName();

	if (bSuccess && !Base64String.IsEmpty())
	{
		FBase64::Decode(Base64String, SaveData.InventoryData);
	}

	PendingLoadDelegate.ExecuteIfBound(bSuccess, SaveData);
}
