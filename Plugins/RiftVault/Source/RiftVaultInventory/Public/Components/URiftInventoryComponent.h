#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Data/URiftItemDefinition.h"
#include "Data/URiftContainerDefinition.h"
#include "GameFramework/Containers/URiftContainer.h"
#include "GameFramework/Items/URiftItemInstance.h"
#include "Interfaces/IRiftPersistenceInterface.h"
#include "URiftInventoryComponent.generated.h"

class ARiftPickup;

// ------------------------------------------------------------------
// Delegates
// ------------------------------------------------------------------

/** Fired when a new container is created and added to this inventory. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRiftContainerAdded, URiftContainer*, Container);

/** Fired when a container (and all its items) is removed from this inventory. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRiftContainerRemoved, URiftContainer*, Container);

/** Fired when an item instance is placed into a container. Carries both the item and its container. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRiftItemAdded, URiftItemInstance*, Item, URiftContainer*, Container);

/** Fired when an item instance is fully removed from the inventory. Carries the item and the container it was in. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRiftItemRemoved, URiftItemInstance*, Item, URiftContainer*, Container);

/**
 * Fired when an item is relocated — either within the same container (slot swap)
 * or across containers. FromContainer and ToContainer may be the same object for
 * intra-container slot moves.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRiftItemMoved, URiftItemInstance*, Item, URiftContainer*, FromContainer, URiftContainer*, ToContainer);

/** Fired once when the async persistence load completes. bSuccess indicates whether data was loaded cleanly. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRiftInventoryInitialized, bool, bSuccess);

/**
 * Broadcast when a gameplay event is dispatched for a specific item.
 * Equipment, Durability, and other systems listen here to react to item lifecycle changes.
 * Also routes to any fragment on the item that has registered the event tag in WatchedEventTags.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRiftItemEvent, URiftItemInstance*, Item, FGameplayTag, EventTag);

/** Non-dynamic delegate for C++ callers using WaitForInitialized. */
DECLARE_DELEGATE_OneParam(FOnRiftInventoryInitializedDelegate, bool);

/** Dynamic single-cast delegate for Blueprint callers using K2_WaitForInitialized. */
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRiftInventoryInitializedBPDelegate, bool, bSuccess);

// ------------------------------------------------------------------
// Fast-array types for the component's tracking lists
// ------------------------------------------------------------------

USTRUCT()
struct FRiftContainerEntry : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<URiftContainer> Container;
};

USTRUCT()
struct FRiftContainerList : public FFastArraySerializer
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FRiftContainerEntry> Entries;

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
};

template<>
struct TStructOpsTypeTraits<FRiftContainerList> : TStructOpsTypeTraitsBase2<FRiftContainerList>
{
    enum { WithNetDeltaSerializer = true };
};

USTRUCT()
struct FRiftItemEntry : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<URiftItemInstance> Item;
};

USTRUCT()
struct FRiftItemList : public FFastArraySerializer
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FRiftItemEntry> Entries;

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
};

template<>
struct TStructOpsTypeTraits<FRiftItemList> : TStructOpsTypeTraitsBase2<FRiftItemList>
{
    enum { WithNetDeltaSerializer = true };
};

/**
 * Defines a single item entry in the DefaultItems starting loadout.
 * Set these on the inventory component's Blueprint defaults to grant items
 * to every new player (or any player whose inventory is empty after load).
 */
USTRUCT(BlueprintType)
struct FRiftDefaultItem
{
    GENERATED_BODY()

    /** The item type to grant. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default Item")
    TObjectPtr<URiftItemDefinition> Definition = nullptr;

    /** How many units to grant. Must be > 0. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default Item")
    int32 Quantity = 1;
};

/**
 * Manages all inventory containers and items for one player.
 *
 * Lives on APlayerState — not APawn. Permanent, closed decision.
 * Inventory survives pawn death and respawn because PlayerState persists across
 * respawns while Pawns are destroyed and recreated.
 *
 * All mutations are server-authoritative. In-session operations are
 * synchronous. The initial persistence load is async — callers that need
 * inventory data at startup should use WaitForInitialized().
 *
 * Container and item subobjects are replicated to the owning client only via
 * the registered subobject list (AddReplicatedSubObject). Non-owner clients
 * receive no inventory data.
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(RiftVault), meta=(BlueprintSpawnableComponent))
class RIFTVAULTINVENTORY_API URiftInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    /** Fired when a new container is successfully created and registered. */
    UPROPERTY(BlueprintAssignable, Category = "RiftVault|Inventory|Delegates")
    FOnRiftContainerAdded OnContainerAdded;

    /** Fired when a container is removed along with all items it held. */
    UPROPERTY(BlueprintAssignable, Category = "RiftVault|Inventory|Delegates")
    FOnRiftContainerRemoved OnContainerRemoved;

    /** Fired each time an item instance lands in a container slot. */
    UPROPERTY(BlueprintAssignable, Category = "RiftVault|Inventory|Delegates")
    FOnRiftItemAdded OnItemAdded;

    /** Fired each time an item instance is fully removed from the inventory. */
    UPROPERTY(BlueprintAssignable, Category = "RiftVault|Inventory|Delegates")
    FOnRiftItemRemoved OnItemRemoved;

    /** Fired when an item changes slot or container. FromContainer == ToContainer for slot swaps. */
    UPROPERTY(BlueprintAssignable, Category = "RiftVault|Inventory|Delegates")
    FOnRiftItemMoved OnItemMoved;

    /** Fired once when the async persistence load resolves. Safe to call before or after loading. */
    UPROPERTY(BlueprintAssignable, Category = "RiftVault|Inventory|Delegates")
    FOnRiftInventoryInitialized OnInventoryInitialized;

    /**
     * Broadcast when a gameplay event is dispatched for a specific item.
     * Fired after the event has been routed to all relevant fragments.
     * Server-side only — external systems (Equipment, Durability) bind here
     * to react to item lifecycle events without polling.
     */
    UPROPERTY(BlueprintAssignable, Category = "RiftVault|Inventory|Delegates")
    FOnRiftItemEvent OnItemEvent;

    URiftInventoryComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // -- Begin UActorComponent interface
    virtual void InitializeComponent() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    // -- End UActorComponent interface

    // -- Begin UObject replication interface
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    // -- End UObject replication interface

    /**
     * Returns true once the async persistence load has completed (successfully or not).
     * Safe to call from any context — does not imply items were loaded, only that the
     * initialization flow finished.
     */
    UFUNCTION(BlueprintPure, Category = "RiftVault|Inventory")
    bool IsInitialized() const;

    /**
     * Calls the delegate immediately if the inventory is already initialized.
     * Otherwise binds the delegate to fire when initialization completes.
     * This is the correct way for any system to wait for inventory readiness
     * regardless of whether it registers before or after initialization fires.
     *
     * Use this instead of binding to OnInventoryInitialized when the caller
     * may register after BeginPlay has already completed.
     *
     * @param Delegate  C++ delegate to invoke with bSuccess once initialized.
     */
    void WaitForInitialized(FOnRiftInventoryInitializedDelegate&& Delegate);

    /**
     * Blueprint equivalent of WaitForInitialized.
     * Executes the delegate immediately if the inventory is already initialized,
     * otherwise queues it to fire when initialization completes.
     * Use this in Blueprint instead of binding to OnInventoryInitialized to avoid
     * missing the event when the widget is added after initialization has already fired.
     *
     * @param Delegate  Blueprint delegate to invoke with bSuccess once initialized.
     */
    UFUNCTION(BlueprintCallable, Category = "RiftVault|Inventory", meta = (DisplayName = "Wait For Initialized"))
    void K2_WaitForInitialized(FOnRiftInventoryInitializedBPDelegate Delegate);

    /**
     * Sets the persistence backend. Must implement IRiftPersistenceInterface.
     * Call before BeginPlay — typically in the owning PlayerState constructor
     * or before the component's BeginPlay executes.
     *
     * @param NewPersistenceObject  An object that implements IRiftPersistenceInterface.
     */
    UFUNCTION(BlueprintCallable, Category = "RiftVault|Inventory")
    void SetPersistenceObject(UObject* NewPersistenceObject);

    // ------------------------------------------------------------------
    // Container API
    // ------------------------------------------------------------------

    /**
     * Creates a new container from the given definition and adds it to this inventory.
     * Server only. Fires OnContainerAdded on success.
     *
     * @param ContainerDefinition  The definition describing the container's rules and capacity.
     * @return  The FGuid of the new container, or an invalid guid if creation failed.
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "RiftVault|Inventory|Containers")
    FGuid AddContainer(URiftContainerDefinition* ContainerDefinition);

    /**
     * Removes the given container from the inventory. All items in the container
     * are also removed (and their fragments deactivated) before the container is destroyed.
     * Server only. Fires OnItemRemoved for each item, then OnContainerRemoved.
     *
     * @param Container  The container to remove.
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "RiftVault|Inventory|Containers")
    void RemoveContainer(URiftContainer* Container);

    /**
     * Finds a container by its unique instance ID.
     *
     * @param ContainerId  The FGuid assigned when the container was created.
     * @return  The matching container, or null if not found.
     */
    UFUNCTION(BlueprintPure, Category = "RiftVault|Inventory|Containers")
    URiftContainer* GetContainerById(const FGuid& ContainerId) const;

    /**
     * Finds the first container whose definition has the given gameplay tag.
     * Useful when containers are identified semantically (e.g. Tag_Rift_Container_Backpack).
     *
     * @param ContainerTag  The gameplay tag to search for.
     * @return  The first matching container, or null if not found.
     */
    UFUNCTION(BlueprintPure, Category = "RiftVault|Inventory|Containers")
    URiftContainer* GetContainerByTag(FGameplayTag ContainerTag) const;

    /**
     * Finds the first container whose definition asset pointer matches the given definition.
     * Use this instead of GetContainerByTag when multiple containers may share the same tag
     * (e.g. two Backpack containers) — the data asset reference uniquely identifies each one.
     *
     * @param Definition  The container definition asset to match against.
     * @return  The first container whose definition pointer equals Definition, or null if not found.
     */
    UFUNCTION(BlueprintPure, Category = "RiftVault|Inventory|Containers")
    URiftContainer* GetContainerByDefinition(const URiftContainerDefinition* Definition) const;

    /**
     * Returns all valid containers currently registered in this inventory.
     * Null entries in the internal array are skipped.
     */
    UFUNCTION(BlueprintPure, Category = "RiftVault|Inventory|Containers")
    TArray<URiftContainer*> GetAllContainers() const;

    /** Returns the total number of containers registered (includes any that may be pending GC). */
    UFUNCTION(BlueprintPure, Category = "RiftVault|Inventory|Containers")
    int32 CountContainers() const;

    // ------------------------------------------------------------------
    // Item API
    // ------------------------------------------------------------------

    /**
     * Adds the given item type to the best available container in this inventory.
     *
     * Stacking strategy:
     *   1. If the definition has a Stack fragment, tries to top-up existing partial stacks first.
     *   2. Creates new item instances in the first available container slot for any remainder.
     *
     * Server only. Fires OnItemAdded and triggers a debounced save.
     *
     * @param Definition  The item type to add.
     * @param Quantity    Number of units to add. Must be > 0.
     * @return  The FGuid of the first item instance created or modified, or invalid if nothing was added.
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "RiftVault|Inventory|Items")
    FGuid AddItem(URiftItemDefinition* Definition, int32 Quantity = 1);

    /**
     * Adds the given item type specifically into the provided container.
     * Follows the same stack-fill-then-create strategy as AddItem but restricts
     * the search to only the given container.
     *
     * Server only.
     *
     * @param Definition       The item type to add.
     * @param TargetContainer  The container to add items into.
     * @param Quantity         Number of units to add. Must be > 0.
     * @return  The FGuid of the first item instance created or modified, or invalid if nothing was added.
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "RiftVault|Inventory|Items")
    FGuid AddItemToContainer(URiftItemDefinition* Definition, URiftContainer* TargetContainer, int32 Quantity = 1);

    /**
     * Removes the given item instance from the inventory entirely.
     * Broadcasts Tag_Rift_Event_Item_Removed, deactivates all fragments, then fires OnItemRemoved.
     * Server only.
     *
     * @param Item  The item to remove.
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "RiftVault|Inventory|Items")
    void RemoveItem(URiftItemInstance* Item);

    /**
     * Moves the given item to a specific slot within its current container.
     * If the target slot is occupied the two items are swapped. Server only.
     *
     * @param Item             The item to move.
     * @param TargetSlotIndex  The destination slot index within the item's current container.
     * @return  true if the move succeeded.
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "RiftVault|Inventory|Items")
    bool MoveItemToSlot(URiftItemInstance* Item, int32 TargetSlotIndex);

    /**
     * Moves the given item from its current container to a different container.
     * If the target container rejects the item or is full the item is returned to
     * its original container and the method returns false. Server only.
     *
     * @param Item             The item to move.
     * @param TargetContainer  The destination container.
     * @return  true if the item was successfully placed in the target container.
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "RiftVault|Inventory|Items")
    bool MoveItemToContainer(URiftItemInstance* Item, URiftContainer* TargetContainer);

    /**
     * Moves the given item from its current container to a specific slot in a different container.
     * If the target slot is occupied the two items are swapped across containers.
     * If the target container rejects the item the move is aborted. Server only.
     *
     * @param Item              The item to move.
     * @param TargetContainer   The destination container.
     * @param TargetSlotIndex   The destination slot index within TargetContainer.
     * @return  true if the item was successfully placed at the target slot.
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "RiftVault|Inventory|Items")
    bool MoveItemToContainerAtSlot(URiftItemInstance* Item, URiftContainer* TargetContainer, int32 TargetSlotIndex);

    /**
     * Server RPC — moves an item to a specific slot within its current container.
     * Uses container tag + slot index as identity (no UObject pointer over the wire).
     */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "RiftVault|Inventory|Items")
    void Server_MoveItemToSlot(FGameplayTag ContainerTag, int32 SourceSlotIndex, int32 TargetSlotIndex);

    /**
     * Server RPC — moves an item from one container to a specific slot in another container.
     * Uses container tag + slot index as identity (no UObject pointer over the wire).
     */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "RiftVault|Inventory|Items")
    void Server_MoveItemToContainerAtSlot(FGameplayTag SourceContainerTag, int32 SourceSlotIndex, FGameplayTag TargetContainerTag, int32 TargetSlotIndex);

    /**
     * Server RPC — moves an item from one container to a specific slot in another container.
     * Uses direct URiftContainer object pointers (valid because containers are replicated subobjects
     * the owning client already holds). Eliminates tag-ambiguity when multiple containers share a tag.
     */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "RiftVault|Inventory|Items")
    void Server_MoveItemToContainerAtSlotByObject(URiftContainer* SourceContainer, int32 SourceSlotIndex, URiftContainer* TargetContainer, int32 TargetSlotIndex);

    /**
     * Server RPC — moves an item to a specific slot within the same container.
     * Uses direct URiftContainer object pointer to eliminate tag-ambiguity when multiple
     * containers share the same tag (e.g. two Backpack containers).
     */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "RiftVault|Inventory|Items")
    void Server_MoveItemToSlotByObject(URiftContainer* Container, int32 SourceSlotIndex, int32 TargetSlotIndex);

    /**
     * Server RPC — merges Quantity units from the source slot into the target slot. Pass -1 for full stack.
     * Uses container tag + slot index as identity (no UObject pointer over the wire).
     */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "RiftVault|Inventory|Items")
    void Server_MergeStacks(FGameplayTag SourceContainerTag, int32 SourceSlotIndex, FGameplayTag TargetContainerTag, int32 TargetSlotIndex, int32 Quantity);

    /**
     * Server RPC — removes the item at the given slot from this inventory and spawns an ARiftPickup
     * in the world near the owning pawn's feet.
     *
     * The item must have URiftFragment_Drop with bDroppable = true or the call is a no-op.
     * Call this from client-side UI widgets instead of calling drop logic directly.
     *
     * @param ContainerTag        Container holding the item to drop.
     * @param SlotIndex           Slot index within that container.
     * @param ConsolidationRadius Radius (cm) to search for an existing ARiftPickup to merge into.
     *                            Pass 0 to always spawn a new pickup.
     * @param PickupClass         Actor class to spawn. Defaults to ARiftPickup if null.
     */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "RiftVault|Inventory|Items",
        meta=(AdvancedDisplay="ConsolidationRadius,PickupClass"))
    void Server_DropItem(FGameplayTag ContainerTag, int32 SlotIndex, float ConsolidationRadius, TSubclassOf<ARiftPickup> PickupClass);

    /**
     * Server RPC — drops the item using a direct container pointer instead of a tag.
     * Eliminates tag-ambiguity when multiple containers share a tag.
     * Preferred over Server_DropItem when the caller already holds the container pointer.
     */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "RiftVault|Inventory|Items",
        meta=(AdvancedDisplay="ConsolidationRadius,PickupClass"))
    void Server_DropItemByObject(URiftContainer* Container, int32 SlotIndex, float ConsolidationRadius, TSubclassOf<ARiftPickup> PickupClass);

    /**
     * Server RPC — permanently removes the item at the given slot from this inventory with no world spawn.
     *
     * The item must have URiftFragment_Drop with bDeletable = true, or have no
     * URiftFragment_Drop at all, or the call is a no-op.
     * Call this from client-side UI widgets instead of calling RemoveItem directly.
     *
     * @param ContainerTag  Container holding the item to delete.
     * @param SlotIndex     Slot index within that container.
     */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "RiftVault|Inventory|Items")
    void Server_DeleteItem(FGameplayTag ContainerTag, int32 SlotIndex);

    /**
     * Server RPC — deletes the item using a direct container pointer instead of a tag.
     * Eliminates tag-ambiguity when multiple containers share a tag.
     * Preferred over Server_DeleteItem when the caller already holds the container pointer.
     */
    UFUNCTION(Server, Reliable, BlueprintCallable, Category = "RiftVault|Inventory|Items")
    void Server_DeleteItemByObject(URiftContainer* Container, int32 SlotIndex);

    /**
     * Splits a stackable item into two stacks.
     *
     * Takes AmountToSplit units from Item and creates a new item instance with
     * that quantity in the same container. The original item retains the remainder.
     * The new instance is placed in the first available slot.
     *
     * Fails if:
     *   - The item has no URiftFragment_Stack.
     *   - AmountToSplit is not in the range [1, CurrentQuantity - 1] (must leave
     *     at least 1 unit in the original stack).
     *   - The container has no free slot to hold the new stack.
     *
     * Server only.
     *
     * @param Item           The stack to split.
     * @param AmountToSplit  Number of units to move to the new stack.
     * @return  The FGuid of the newly created item instance, or an invalid guid if the split failed.
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "RiftVault|Inventory|Items")
    FGuid SplitStack(URiftItemInstance* Item, int32 AmountToSplit);

    /**
     * Finds an item instance by its unique ID.
     *
     * @param ItemId  The FGuid to search for.
     * @return  The matching item instance, or null if not found.
     */
    UFUNCTION(BlueprintPure, Category = "RiftVault|Inventory|Items")
    URiftItemInstance* GetItemById(const FGuid& ItemId) const;

    /**
     * Finds which container currently holds the given item.
     *
     * @param Item  The item to locate.
     * @return  The container holding the item, or null if not found in any container.
     */
    UFUNCTION(BlueprintPure, Category = "RiftVault|Inventory|Items")
    URiftContainer* GetContainerForItem(const URiftItemInstance* Item) const;

    /**
     * Returns true if the inventory contains at least one item of the given type.
     *
     * @param Definition  The item type to check for.
     */
    UFUNCTION(BlueprintPure, Category = "RiftVault|Inventory|Items")
    bool HasItem(const URiftItemDefinition* Definition) const;

    /**
     * Returns all item instances whose definition matches the given one.
     *
     * @param Definition  The item type to filter by.
     * @return  All matching instances. May be empty.
     */
    UFUNCTION(BlueprintPure, Category = "RiftVault|Inventory|Items")
    TArray<URiftItemInstance*> GetItemsByDefinition(const URiftItemDefinition* Definition) const;

    /**
     * Returns all valid item instances currently in this inventory across all containers.
     * Null entries in the internal array are skipped.
     */
    UFUNCTION(BlueprintPure, Category = "RiftVault|Inventory|Items")
    TArray<URiftItemInstance*> GetAllItems() const;

    /** Returns the total item count (includes any that may be pending GC). */
    UFUNCTION(BlueprintPure, Category = "RiftVault|Inventory|Items")
    int32 CountItems() const;

    /**
     * Dispatches a gameplay event for the given item.
     *
     * Routes the event to all fragments on the item whose WatchedEventTags
     * includes the given EventTag, then broadcasts OnItemEvent for external
     * server-side listeners (Equipment component, Durability, etc.).
     *
     * Must only be called server-side. Use the pre-defined tags in RiftVaultTags.h
     * (e.g. Tag_Rift_Event_Item_Equipped, Tag_Rift_Event_Item_StackChanged).
     *
     * @param Item      The item receiving the event.
     * @param EventTag  The gameplay tag identifying the event.
     */
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "RiftVault|Inventory|Items")
    void BroadcastItemEvent(URiftItemInstance* Item, FGameplayTag EventTag);

    // ------------------------------------------------------------------
    // Client-side reconstruction helpers (called by FRiftSlotList callbacks)
    // ------------------------------------------------------------------

    /**
     * Creates a new URiftItemInstance on the client by reading the slot descriptor.
     * Called from FRiftSlotList::PostReplicatedAdd / PostReplicatedChange.
     * Initializes fragment states locally (bypasses authority check via bReconstructingLocally).
     *
     * @param Definition  The item's type. Must be valid.
     * @param Quantity    Stack size to initialize the item with.
     * @return  A freshly created item instance, or null if Definition is invalid.
     */
    URiftItemInstance* ReconstructItemInstance(URiftItemDefinition* Definition, int32 Quantity);

    /**
     * Called when a reconstructed item is no longer needed on the client
     * (slot cleared or definition replaced). Hook for future client-side cache cleanup.
     *
     * @param Item  The item to evict. Caller is responsible for clearing the slot's Item pointer.
     */
    void EvictItemFromCache(URiftItemInstance* Item);

    /**
     * Adjusts the local stack quantity on a reconstructed item instance to match the
     * replicated Quantity field. Called from FRiftSlotList::PostReplicatedChange when the
     * definition is unchanged but the quantity has updated.
     *
     * @param Item        The item to update.
     * @param NewQuantity The new quantity received from the server.
     */
    void UpdateItemInstanceQuantity(URiftItemInstance* Item, int32 NewQuantity);

    /**
     * Syncs the replicated Quantity field in the slot descriptor to the item's current stack size.
     * Called server-side after item initialization and whenever Tag_Rift_Event_Item_StackChanged fires.
     * Marks the slot entry dirty so the updated quantity replicates to owning clients.
     *
     * @param Item  The item whose slot descriptor needs refreshing.
     */
    void RefreshSlotDescriptor(URiftItemInstance* Item);

protected:

    /**
     * Containers to create automatically on BeginPlay (server only).
     * Configured in the component's defaults in the editor.
     * Defines the "shape" of this player's inventory before any persistence data is applied.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
    TArray<TObjectPtr<URiftContainerDefinition>> DefaultContainers;

    /**
     * Items to grant automatically after initialization, server only.
     * Only applied when the inventory is empty after loading (new players or no save data).
     * Existing players with saved items are unaffected.
     * Configure in the component's Blueprint defaults.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
    TArray<FRiftDefaultItem> DefaultItems;

    /**
     * Called once on server BeginPlay. Creates default containers and, if a
     * persistence backend is configured, kicks off the async load.
     */
    virtual void InitializeInventory();

    /**
     * Async callback from the persistence backend once the load resolves.
     * Populates the inventory from SaveData on success, then fires BroadcastInitialized.
     *
     * @param bSuccess   True if the save data was loaded without errors.
     * @param SaveData   The deserialized save data. Only meaningful when bSuccess is true.
     */
    UFUNCTION()
    virtual void OnInventoryLoaded(bool bSuccess, const FRiftInventorySaveData& SaveData);

    /**
     * Schedules a debounced write to the persistence backend.
     * The 2-second debounce prevents multiple rapid mutations (e.g. adding a full stack)
     * from each triggering a separate save. Only fires if a persistence object is set.
     */
    virtual void SaveInventory();

private:

    /**
     * All containers currently registered in this inventory. Owned by this component.
     * Replicated to the owning client only via delta serialization.
     */
    UPROPERTY(Replicated)
    FRiftContainerList Containers;

    /**
     * All item instances currently in this inventory. Owned by this component.
     * Replicated to the owning client only via delta serialization.
     */
    UPROPERTY()
    FRiftItemList Items;

    /** Optional persistence backend. Must implement IRiftPersistenceInterface. May be null. */
    UPROPERTY()
    TObjectPtr<UObject> PersistenceObject;

    /**
     * Set to true once InitializeInventory has fully completed (load callback fired).
     * Replicated to the owning client with an OnRep so WaitForInitialized callbacks
     * fire correctly on clients that register after replication arrives.
     */
    UPROPERTY(ReplicatedUsing = OnRep_bIsInitialized)
    bool bIsInitialized = false;

    /**
     * Called on the owning client when bIsInitialized replicates from the server.
     * Drains any pending WaitForInitialized delegates so client-side widgets
     * that registered before replication arrived get their callbacks.
     */
    UFUNCTION()
    void OnRep_bIsInitialized();

    /**
     * Timer handle for the debounced save. Cleared on EndPlay to prevent a callback
     * from firing after the component has been destroyed.
     */
    FTimerHandle SaveDebounceHandle;

    /**
     * C++ delegates queued by WaitForInitialized callers that registered before
     * initialization completed. Drained into LocalPending and cleared atomically
     * in BroadcastInitialized to avoid re-entrancy issues.
     */
    TArray<FOnRiftInventoryInitializedDelegate> PendingInitializedDelegates;

    /**
     * Sets bIsInitialized, fires OnInventoryInitialized, then drains all
     * PendingInitializedDelegates. The pending list is moved (not iterated in-place)
     * so delegates added during callbacks are handled correctly.
     *
     * @param bSuccess  Passed through to all listeners.
     */
    void BroadcastInitialized(bool bSuccess);

    /**
     * Adds each entry from DefaultItems via AddItem.
     * No-op if the inventory already contains items — prevents re-granting
     * the starting loadout to players whose save data has already been loaded.
     */
    void AddDefaultItems();

    /**
     * Allocates a new URiftItemInstance, assigns a fresh FGuid, and links it to the definition.
     * Does NOT add the instance to Containers or Items — the caller does that.
     *
     * @param Definition  The item type to instantiate.
     * @return  A newly created item instance owned by this component.
     */
    URiftItemInstance* CreateItemInstance(URiftItemDefinition* Definition);

    /**
     * Allocates a new URiftContainer, assigns a fresh FGuid, links it to the definition,
     * pre-allocates the slot array, adds it to Containers, and fires OnContainerAdded.
     *
     * @param Definition  The container definition to use.
     * @return  The new container.
     */
    URiftContainer* CreateContainer(URiftContainerDefinition* Definition);

    /**
     * Iterates Containers in order and returns the first one that passes CanAcceptItem.
     * "Best" here means "first available" — no priority scoring is performed.
     *
     * @param Item  The item looking for a home.
     * @return  A container that will accept the item, or null if none found.
     */
    URiftContainer* FindBestContainerForItem(const URiftItemInstance* Item) const;

    /**
     * Finds the container with the given tag that actually holds an item at the specified slot.
     * Needed because GetContainerByTag returns the first tag match, which breaks when
     * multiple containers share a tag (e.g. Bag_1 / Bag_2).
     *
     * @param ContainerTag  The gameplay tag to match.
     * @param SlotIndex     The slot that must contain a valid item.
     * @return  The matching container, or null if no container with that tag has an item at that slot.
     */
    URiftContainer* FindContainerWithItemAtSlot(FGameplayTag ContainerTag, int32 SlotIndex) const;

    /**
     * Initializes a freshly created item: fragment states, stack quantity, activation,
     * slot descriptor sync, and broadcasts. Returns the remaining quantity (overflow
     * from stacking, or RemainingQuantity - 1 for non-stackable items).
     *
     * @param NewItem            The item to finalize. Must already be placed in a container slot.
     * @param Container          The container holding the item (for OnItemAdded broadcast).
     * @param RemainingQuantity  Units still to place. The item consumes as many as it can.
     * @return  Leftover units that did not fit in this item's stack.
     */
    int32 FinalizeNewItem(URiftItemInstance* NewItem, URiftContainer* Container, int32 RemainingQuantity);
};
