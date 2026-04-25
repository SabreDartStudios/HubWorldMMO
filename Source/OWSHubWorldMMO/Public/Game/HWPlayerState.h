// Copyright 2023 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "OWSPlayerState.h"
#include "Interfaces/IRiftPersistenceInterface.h"
#include "Components/URiftInventoryComponent.h"
#include "HWPlayerState.generated.h"

/**
 * HWPlayerState owns the RiftVault inventory component and implements the
 * persistence backend, bridging RiftVault save/load calls to OWS CustomCharacterData.
 */
UCLASS()
class OWSHUBWORLDMMO_API AHWPlayerState : public AOWSPlayerState, public IRiftPersistenceInterface
{
	GENERATED_BODY()

public:

	AHWPlayerState();

	/** The player's inventory. Lives here (not on the Pawn) to survive respawns. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
		URiftInventoryComponent* InventoryComponent;

	/**
	 * Called by AHWPlayerController::NotifyGetCustomCharacterData when the
	 * "RiftInventory" custom data row arrives (or is absent for a new player).
	 *
	 * @param bSuccess     True when a saved inventory row was found and decoded.
	 * @param Base64String The Base64-encoded inventory blob, or empty for a new player.
	 */
	void DeliverInventoryLoadResult(bool bSuccess, const FString& Base64String);

	// -- IRiftPersistenceInterface --
	virtual void SaveInventory_Implementation(const FRiftInventorySaveData& Data, const FOnSaveComplete& OnComplete) override;
	virtual void LoadInventory_Implementation(const FString& PlayerId, const FOnLoadComplete& OnComplete) override;

private:

	/** Stored by LoadInventory_Implementation; fired by DeliverInventoryLoadResult. */
	FOnLoadComplete PendingLoadDelegate;

	// Delivery cache — guards against DeliverInventoryLoadResult arriving before
	// LoadInventory_Implementation has stored PendingLoadDelegate (timing race on fast loopback).
	bool bHasCachedResult = false;
	bool bCachedSuccess = false;
	FString CachedInventoryData;
};
