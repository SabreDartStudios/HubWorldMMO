#include "Components/URiftInventoryComponent.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Interfaces/IRiftPersistenceInterface.h"
#include "Tags/RiftVaultTags.h"
#include "GameFramework/Fragments/URiftFragment_Stack.h"
#include "GameFramework/Fragments/URiftFragment_Drop.h"
#include "Actors/ARiftPickup.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#if ENGINE_MINOR_VERSION < 5
#include "InstancedStruct.h"
#else
#include "StructUtils/InstancedStruct.h"
#endif
#include "Types/State/FRiftStackState.h"

bool FRiftContainerList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
    return FFastArraySerializer::FastArrayDeltaSerialize<FRiftContainerEntry, FRiftContainerList>(Entries, DeltaParams, *this);
}

bool FRiftItemList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
    return FFastArraySerializer::FastArrayDeltaSerialize<FRiftItemEntry, FRiftItemList>(Entries, DeltaParams, *this);
}

URiftInventoryComponent::URiftInventoryComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsReplicatedByDefault(true);
    bWantsInitializeComponent = true;
    bReplicateUsingRegisteredSubObjectList = true;
}

void URiftInventoryComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void URiftInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwner() && GetOwner()->HasAuthority())
    {
        InitializeInventory();
    }
}

void URiftInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SaveDebounceHandle);
    }
    Super::EndPlay(EndPlayReason);
}

void URiftInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Containers replicate to the owning client — each container carries its slot descriptor array
    // (ItemDefinition + Quantity per slot) which the client uses to reconstruct item instances locally.
    // Items is server-side only (stack-fill index); it is no longer replicated.
    DOREPLIFETIME_CONDITION(URiftInventoryComponent, Containers,     COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(URiftInventoryComponent, bIsInitialized, COND_OwnerOnly);
}


bool URiftInventoryComponent::IsInitialized() const
{
    return bIsInitialized;
}

void URiftInventoryComponent::WaitForInitialized(FOnRiftInventoryInitializedDelegate&& Delegate)
{
    if (bIsInitialized)
    {
        UE_LOG(LogTemp, Log, TEXT("URiftInventoryComponent::WaitForInitialized — already initialized, calling immediately."));
        Delegate.Execute(true);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("URiftInventoryComponent::WaitForInitialized — not yet initialized, queuing delegate."));
    PendingInitializedDelegates.Add(MoveTemp(Delegate));
}

void URiftInventoryComponent::K2_WaitForInitialized(FOnRiftInventoryInitializedBPDelegate Delegate)
{
    if (!Delegate.IsBound())
    {
        return;
    }

    if (bIsInitialized)
    {
        Delegate.Execute(true);
        return;
    }

    // Wrap the dynamic delegate in a non-dynamic one and queue it.
    WaitForInitialized(FOnRiftInventoryInitializedDelegate::CreateLambda(
        [Delegate](bool bSuccess) mutable
        {
            if (Delegate.IsBound())
            {
                Delegate.Execute(bSuccess);
            }
        }));
}

void URiftInventoryComponent::BroadcastInitialized(bool bSuccess)
{
    UE_LOG(LogTemp, Log, TEXT("URiftInventoryComponent::BroadcastInitialized — bSuccess: %s, pending delegates: %d"), bSuccess ? TEXT("true") : TEXT("false"), PendingInitializedDelegates.Num());
    OnInventoryInitialized.Broadcast(bSuccess);

    TArray<FOnRiftInventoryInitializedDelegate> LocalPending = MoveTemp(PendingInitializedDelegates);
    for (FOnRiftInventoryInitializedDelegate& Delegate : LocalPending)
    {
        if (Delegate.IsBound())
        {
            Delegate.Execute(bSuccess);
        }
    }
}

void URiftInventoryComponent::OnRep_bIsInitialized()
{
    // bIsInitialized just arrived on the owning client.
    // Drain any delegates that were queued before replication arrived
    // (e.g. a UI widget that called WaitForInitialized before the first net update).
    if (bIsInitialized)
    {
        BroadcastInitialized(true);
    }
}

void URiftInventoryComponent::AddDefaultItems()
{
    if (DefaultItems.IsEmpty())
    {
        return;
    }

    // If the inventory already has items (e.g. from a save load), skip granting.
    // This prevents re-adding the starting loadout every time an existing player logs in.
    if (!Items.Entries.IsEmpty())
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("URiftInventoryComponent::AddDefaultItems — granting %d default item entries."), DefaultItems.Num());

    for (const FRiftDefaultItem& Entry : DefaultItems)
    {
        if (IsValid(Entry.Definition) && Entry.Quantity > 0)
        {
            AddItem(Entry.Definition, Entry.Quantity);
        }
    }
}

void URiftInventoryComponent::SetPersistenceObject(UObject* NewPersistenceObject)
{
    PersistenceObject = NewPersistenceObject;
}

void URiftInventoryComponent::DeliverLoadResult(bool bSuccess, const FRiftInventorySaveData& SaveData)
{
    OnInventoryLoaded(bSuccess, SaveData);
}

void URiftInventoryComponent::InitializeInventory()
{
    UE_LOG(LogTemp, Log, TEXT("URiftInventoryComponent::InitializeInventory — creating %d default containers."), DefaultContainers.Num());

    for (URiftContainerDefinition* Definition : DefaultContainers)
    {
        if (IsValid(Definition))
        {
            URiftContainer* Container = CreateContainer(Definition);
            UE_LOG(LogTemp, Log, TEXT("URiftInventoryComponent — Created container with tag: %s, capacity: %d"),
                *Container->GetContainerTag().ToString(),
                Container->GetCapacity());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("URiftInventoryComponent — Null container definition in DefaultContainers array."));
        }
    }

    if (IsValid(PersistenceObject) && PersistenceObject->Implements<URiftPersistenceInterface>())
    {
        FString PlayerId;
        if (APlayerState* PlayerState = Cast<APlayerState>(GetOwner()))
        {
            PlayerId = PlayerState->GetPlayerName();
        }

        FOnLoadComplete LoadCallback;
        LoadCallback.BindDynamic(this, &URiftInventoryComponent::OnInventoryLoaded);
        IRiftPersistenceInterface::Execute_LoadInventory(PersistenceObject, PlayerId, LoadCallback);
    }
    else
    {
        // No persistence — grant default items before signalling readiness.
        AddDefaultItems();
        bIsInitialized = true;
        BroadcastInitialized(true);
    }
}

void URiftInventoryComponent::OnInventoryLoaded(const bool bSuccess, const FRiftInventorySaveData& SaveData)
{
    if (!bSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("URiftInventoryComponent: Failed to load inventory."));
        AddDefaultItems();
        bIsInitialized = true;
        BroadcastInitialized(false);
        return;
    }

    if (SaveData.InventoryData.Num() > 0)
    {
        // Null-terminate the UTF-8 blob so UTF8_TO_TCHAR is safe, then parse JSON.
        TArray<uint8> Bytes = SaveData.InventoryData;
        Bytes.Add(0);
        const FString JsonString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(Bytes.GetData())));

        TSharedPtr<FJsonObject> Root;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
        if (FJsonSerializer::Deserialize(Reader, Root) && Root.IsValid())
        {
            const TArray<TSharedPtr<FJsonValue>>* ContainerArray;
            if (Root->TryGetArrayField(TEXT("containers"), ContainerArray))
            {
                for (const TSharedPtr<FJsonValue>& ContValue : *ContainerArray)
                {
                    const TSharedPtr<FJsonObject> ContObj = ContValue->AsObject();
                    if (!ContObj.IsValid()) { continue; }

                    FString ContDefPath;
                    if (!ContObj->TryGetStringField(TEXT("def"), ContDefPath)) { continue; }

                    URiftContainerDefinition* ContDef = LoadObject<URiftContainerDefinition>(nullptr, *ContDefPath);
                    URiftContainer* Container = IsValid(ContDef) ? GetContainerByDefinition(ContDef) : nullptr;
                    if (!IsValid(Container))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("URiftInventoryComponent::OnInventoryLoaded — no container for saved def '%s', skipping."), *ContDefPath);
                        continue;
                    }

                    const TArray<TSharedPtr<FJsonValue>>* SlotArray;
                    if (!ContObj->TryGetArrayField(TEXT("slots"), SlotArray)) { continue; }

                    for (int32 SlotIndex = 0; SlotIndex < SlotArray->Num(); ++SlotIndex)
                    {
                        const TSharedPtr<FJsonValue>& SlotValue = (*SlotArray)[SlotIndex];
                        if (!SlotValue.IsValid() || SlotValue->Type == EJson::Null) { continue; }

                        const TSharedPtr<FJsonObject> SlotObj = SlotValue->AsObject();
                        if (!SlotObj.IsValid()) { continue; }

                        FString ItemDefPath;
                        if (!SlotObj->TryGetStringField(TEXT("def"), ItemDefPath)) { continue; }

                        int32 Quantity = 1;
                        SlotObj->TryGetNumberField(TEXT("qty"), Quantity);

                        URiftItemDefinition* ItemDef = LoadObject<URiftItemDefinition>(nullptr, *ItemDefPath);
                        if (!IsValid(ItemDef))
                        {
                            UE_LOG(LogTemp, Warning, TEXT("URiftInventoryComponent::OnInventoryLoaded — item def '%s' not found, skipping slot %d."), *ItemDefPath, SlotIndex);
                            continue;
                        }

                        // Grow the slot array for List-layout containers that don't pre-allocate.
                        if (!Container->Slots.Entries.IsValidIndex(SlotIndex))
                        {
                            Container->Slots.Entries.SetNum(SlotIndex + 1);
                        }

                        if (IsValid(Container->Slots.Entries[SlotIndex].Item))
                        {
                            continue; // slot already occupied — shouldn't happen on a fresh load
                        }

                        URiftItemInstance* NewItem = CreateItemInstance(ItemDef);
                        if (!Container->PlaceItemAtSlot(NewItem, SlotIndex))
                        {
                            continue;
                        }

                        FRiftItemEntry NewEntry;
                        NewEntry.Item = NewItem;
                        Items.Entries.Add(NewEntry);

                        FinalizeNewItem(NewItem, Container, Quantity);
                    }
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("URiftInventoryComponent::OnInventoryLoaded — failed to parse inventory JSON, granting defaults."));
        }
    }

    // AddDefaultItems is a no-op if items were already loaded above (Items.Entries not empty).
    AddDefaultItems();
    bIsInitialized = true;
    BroadcastInitialized(true);
}

void URiftInventoryComponent::SaveInventory()
{
    if (!IsValid(PersistenceObject) || !PersistenceObject->Implements<URiftPersistenceInterface>())
    {
        return;
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SaveDebounceHandle);
        World->GetTimerManager().SetTimer(SaveDebounceHandle, [this]()
        {
            FRiftInventorySaveData SaveData;
            if (APlayerState* PlayerState = Cast<APlayerState>(GetOwner()))
            {
                SaveData.PlayerId = PlayerState->GetPlayerName();
            }

            // Serialize containers and their slot contents to JSON.
            // Format: { "v":1, "containers": [ { "def":"<asset_path>", "slots":[ {"def":"...","qty":N} | null, ... ] }, ... ] }
            TSharedPtr<FJsonObject> Root = MakeShareable(new FJsonObject);
            Root->SetNumberField(TEXT("v"), 1);

            TArray<TSharedPtr<FJsonValue>> ContainerArray;
            for (const FRiftContainerEntry& ContEntry : Containers.Entries)
            {
                URiftContainer* Container = ContEntry.Container.Get();
                if (!IsValid(Container) || !IsValid(Container->GetDefinition()))
                {
                    continue;
                }

                TSharedPtr<FJsonObject> ContObj = MakeShareable(new FJsonObject);
                ContObj->SetStringField(TEXT("def"), Container->GetDefinition()->GetPathName());

                TArray<TSharedPtr<FJsonValue>> SlotArray;
                for (const FRiftSlotEntry& SlotEntry : Container->Slots.Entries)
                {
                    if (IsValid(SlotEntry.ItemDefinition) && SlotEntry.Quantity > 0)
                    {
                        TSharedPtr<FJsonObject> SlotObj = MakeShareable(new FJsonObject);
                        SlotObj->SetStringField(TEXT("def"), SlotEntry.ItemDefinition->GetPathName());
                        SlotObj->SetNumberField(TEXT("qty"), SlotEntry.Quantity);
                        SlotArray.Add(MakeShareable(new FJsonValueObject(SlotObj)));
                    }
                    else
                    {
                        SlotArray.Add(MakeShareable(new FJsonValueNull()));
                    }
                }
                ContObj->SetArrayField(TEXT("slots"), SlotArray);
                ContainerArray.Add(MakeShareable(new FJsonValueObject(ContObj)));
            }
            Root->SetArrayField(TEXT("containers"), ContainerArray);

            FString JsonString;
            TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
            FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);

            FTCHARToUTF8 Converter(*JsonString);
            SaveData.InventoryData.Append(reinterpret_cast<const uint8*>(Converter.Get()), Converter.Length());

            FOnSaveComplete SaveCallback;
            IRiftPersistenceInterface::Execute_SaveInventory(PersistenceObject, SaveData, SaveCallback);

        }, 2.0f, false);
    }
}

FGuid URiftInventoryComponent::AddContainer(URiftContainerDefinition* ContainerDefinition)
{
    if (!IsValid(ContainerDefinition))
    {
        return FGuid();
    }

    URiftContainer* NewContainer = CreateContainer(ContainerDefinition);
    return IsValid(NewContainer) ? NewContainer->GetContainerId() : FGuid();
}

void URiftInventoryComponent::RemoveContainer(URiftContainer* Container)
{
    if (!IsValid(Container))
    {
        return;
    }

    for (URiftItemInstance* Item : Container->GetAllItems())
    {
        RemoveItem(Item);
    }

    RemoveReplicatedSubObject(Container);
    Containers.Entries.RemoveAll([Container](const FRiftContainerEntry& E){ return E.Container == Container; });
    Containers.MarkArrayDirty();
    OnContainerRemoved.Broadcast(Container);
}

URiftContainer* URiftInventoryComponent::GetContainerById(const FGuid& ContainerId) const
{
    for (const FRiftContainerEntry& Entry : Containers.Entries)
    {
        if (IsValid(Entry.Container) && Entry.Container->GetContainerId() == ContainerId)
        {
            return Entry.Container.Get();
        }
    }
    return nullptr;
}

URiftContainer* URiftInventoryComponent::GetContainerByTag(const FGameplayTag ContainerTag) const
{
    for (const FRiftContainerEntry& Entry : Containers.Entries)
    {
        if (IsValid(Entry.Container) && Entry.Container->GetContainerTag() == ContainerTag)
        {
            return Entry.Container.Get();
        }
    }
    return nullptr;
}

URiftContainer* URiftInventoryComponent::GetContainerByDefinition(const URiftContainerDefinition* Definition) const
{
    if (!IsValid(Definition))
    {
        return nullptr;
    }

    for (const FRiftContainerEntry& Entry : Containers.Entries)
    {
        if (IsValid(Entry.Container) && Entry.Container->GetDefinition() == Definition)
        {
            return Entry.Container.Get();
        }
    }
    return nullptr;
}

TArray<URiftContainer*> URiftInventoryComponent::GetAllContainers() const
{
    TArray<URiftContainer*> Result;
    for (const FRiftContainerEntry& Entry : Containers.Entries)
    {
        if (IsValid(Entry.Container))
        {
            Result.Add(Entry.Container.Get());
        }
    }
    return Result;
}

int32 URiftInventoryComponent::CountContainers() const
{
    return Containers.Entries.Num();
}

FGuid URiftInventoryComponent::AddItem(URiftItemDefinition* Definition, const int32 Quantity)
{
    if (!IsValid(Definition) || Quantity <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("URiftInventoryComponent::AddItem — Invalid definition or quantity <= 0."));
        return FGuid();
    }

    int32 RemainingQuantity = Quantity;
    FGuid LastModifiedId;

    // --- Step 1: Fill existing stacks of the same item type first ---
    // Only attempt if this definition has a Stack fragment — avoids iterating
    // all items for non-stackable types.
    if (Definition->FindFragment<URiftFragment_Stack>())
    {
        for (const FRiftItemEntry& ItemEntry : Items.Entries)
        {
            if (RemainingQuantity <= 0)
            {
                break;
            }

            URiftItemInstance* ExistingItem = ItemEntry.Item.Get();
            if (!IsValid(ExistingItem) || ExistingItem->GetDefinition() != Definition)
            {
                continue;
            }

            URiftFragment_Stack* StackFrag = ExistingItem->FindFragment<URiftFragment_Stack>();

            if (!StackFrag || StackFrag->IsFull(ExistingItem))
            {
                continue;
            }

            const int32 Overflow = StackFrag->AddQuantity(ExistingItem, RemainingQuantity);
            RemainingQuantity = Overflow;
            LastModifiedId = ExistingItem->GetItemId();
        }
    }

    // --- Step 2: Create new item instances for any remaining quantity ---
    // Each loop iteration fills one inventory slot as much as MaxStackSize allows.
    // Non-stackable items consume one slot per unit (RemainingQuantity decrements by 1).
    while (RemainingQuantity > 0)
    {
        URiftItemInstance* NewItem = CreateItemInstance(Definition);
        if (!IsValid(NewItem))
        {
            UE_LOG(LogTemp, Warning, TEXT("URiftInventoryComponent::AddItem — Failed to create item instance for %s."), *Definition->GetInternalName().ToString());
            break;
        }

        URiftContainer* TargetContainer = FindBestContainerForItem(NewItem);
        if (!IsValid(TargetContainer))
        {
            UE_LOG(LogTemp, Warning, TEXT("URiftInventoryComponent::AddItem — No container available for item %s. %d units not added."),
                *Definition->GetInternalName().ToString(), RemainingQuantity);
            break;
        }

        const int32 SlotIndex = TargetContainer->AddItem(NewItem);
        if (SlotIndex == INDEX_NONE)
        {
            UE_LOG(LogTemp, Warning, TEXT("URiftInventoryComponent::AddItem — Container rejected item %s at slot add."), *Definition->GetInternalName().ToString());
            break;
        }

        FRiftItemEntry NewEntry;
        NewEntry.Item = NewItem;
        Items.Entries.Add(NewEntry);

        RemainingQuantity = FinalizeNewItem(NewItem, TargetContainer, RemainingQuantity);

        if (!LastModifiedId.IsValid())
        {
            LastModifiedId = NewItem->GetItemId();
        }

        UE_LOG(LogTemp, Log, TEXT("URiftInventoryComponent::AddItem — Added item %s to container %s at slot %d."),
            *Definition->GetInternalName().ToString(),
            *TargetContainer->GetContainerTag().ToString(),
            SlotIndex);
    }

    if (LastModifiedId.IsValid())
    {
        SaveInventory();
    }

    return LastModifiedId;
}

FGuid URiftInventoryComponent::AddItemToContainer(URiftItemDefinition* Definition, URiftContainer* TargetContainer, const int32 Quantity)
{
    if (!IsValid(Definition) || !IsValid(TargetContainer) || Quantity <= 0)
    {
        return FGuid();
    }

    int32 RemainingQuantity = Quantity;
    FGuid LastModifiedId;

    // --- Step 1: Fill existing stacks in the target container first ---
    if (Definition->FindFragment<URiftFragment_Stack>())
    {
        for (const FRiftItemEntry& ItemEntry : Items.Entries)
        {
            if (RemainingQuantity <= 0)
            {
                break;
            }

            URiftItemInstance* ExistingItem = ItemEntry.Item.Get();
            if (!IsValid(ExistingItem) || ExistingItem->GetDefinition() != Definition)
            {
                continue;
            }

            // Only consider items that are in the specified target container.
            if (!TargetContainer->HasItem(ExistingItem))
            {
                continue;
            }

            URiftFragment_Stack* StackFrag = ExistingItem->FindFragment<URiftFragment_Stack>();

            if (!StackFrag || StackFrag->IsFull(ExistingItem))
            {
                continue;
            }

            const int32 Overflow = StackFrag->AddQuantity(ExistingItem, RemainingQuantity);
            RemainingQuantity = Overflow;
            LastModifiedId = ExistingItem->GetItemId();
        }
    }

    // --- Step 2: Create new instances for remaining quantity in the target container ---
    while (RemainingQuantity > 0)
    {
        URiftItemInstance* NewItem = CreateItemInstance(Definition);
        if (!IsValid(NewItem))
        {
            break;
        }

        if (!TargetContainer->CanAcceptItem(NewItem))
        {
            UE_LOG(LogTemp, Warning, TEXT("URiftInventoryComponent::AddItemToContainer — Container full or rejected item %s. %d units not added."),
                *Definition->GetInternalName().ToString(), RemainingQuantity);
            break;
        }

        const int32 SlotIndex = TargetContainer->AddItem(NewItem);
        if (SlotIndex == INDEX_NONE)
        {
            break;
        }

        FRiftItemEntry NewItemEntry;
        NewItemEntry.Item = NewItem;
        Items.Entries.Add(NewItemEntry);

        RemainingQuantity = FinalizeNewItem(NewItem, TargetContainer, RemainingQuantity);

        if (!LastModifiedId.IsValid())
        {
            LastModifiedId = NewItem->GetItemId();
        }
    }

    if (LastModifiedId.IsValid())
    {
        SaveInventory();
    }

    return LastModifiedId;
}

void URiftInventoryComponent::RemoveItem(URiftItemInstance* Item)
{
    if (!IsValid(Item))
    {
        return;
    }

    URiftContainer* Container = GetContainerForItem(Item);

    // Broadcast before deactivation so fragments are still active when they receive the event.
    BroadcastItemEvent(Item, Tag_Rift_Event_Item_Removed);
    Item->DeactivateFragments();

    if (IsValid(Container))
    {
        Container->RemoveItem(Item);
        OnItemRemoved.Broadcast(Item, Container);
    }

    // Items is server-side only; just update the local index.
    Items.Entries.RemoveAll([Item](const FRiftItemEntry& E){ return E.Item == Item; });
    SaveInventory();
}

bool URiftInventoryComponent::MoveItemToSlot(URiftItemInstance* Item, const int32 TargetSlotIndex)
{
    if (!IsValid(Item))
    {
        return false;
    }

    URiftContainer* Container = GetContainerForItem(Item);
    if (!IsValid(Container))
    {
        return false;
    }

    const bool bMoved = Container->MoveItemToSlot(Item, TargetSlotIndex);
    if (bMoved)
    {
        // One broadcast covers both slots — the grid refreshes all changed slots on receipt.
        OnItemMoved.Broadcast(Item, Container, Container);
        SaveInventory();
    }

    return bMoved;
}

bool URiftInventoryComponent::MoveItemToContainer(URiftItemInstance* Item, URiftContainer* TargetContainer)
{
    if (!IsValid(Item) || !IsValid(TargetContainer))
    {
        return false;
    }

    URiftContainer* SourceContainer = GetContainerForItem(Item);
    if (!IsValid(SourceContainer))
    {
        return false;
    }

    if (!TargetContainer->CanAcceptItem(Item))
    {
        return false;
    }

    SourceContainer->RemoveItem(Item);
    const int32 SlotIndex = TargetContainer->AddItem(Item);

    if (SlotIndex == INDEX_NONE)
    {
        SourceContainer->AddItem(Item);
        return false;
    }

    OnItemMoved.Broadcast(Item, SourceContainer, TargetContainer);
    SaveInventory();

    return true;
}

bool URiftInventoryComponent::MoveItemToContainerAtSlot(URiftItemInstance* Item, URiftContainer* TargetContainer, const int32 TargetSlotIndex)
{
    if (!IsValid(Item) || !IsValid(TargetContainer))
    {
        UE_LOG(LogTemp, Warning, TEXT("MoveItemToContainerAtSlot — FAIL: basic validity. Item=%s, Target=%s"),
            *GetNameSafe(Item), *GetNameSafe(TargetContainer));
        return false;
    }

    // For List containers, grow entries on demand up to capacity so empty slots
    // can receive drops without being pre-allocated.
    if (!TargetContainer->Slots.Entries.IsValidIndex(TargetSlotIndex))
    {
        const int32 Capacity = TargetContainer->GetCapacity();
        if (TargetSlotIndex >= Capacity)
        {
            UE_LOG(LogTemp, Warning, TEXT("MoveItemToContainerAtSlot — FAIL: SlotIndex=%d exceeds capacity=%d"), TargetSlotIndex, Capacity);
            return false;
        }
        TargetContainer->Slots.Entries.SetNum(TargetSlotIndex + 1);
    }

    URiftContainer* SourceContainer = GetContainerForItem(Item);
    if (!IsValid(SourceContainer) || SourceContainer == TargetContainer)
    {
        UE_LOG(LogTemp, Warning, TEXT("MoveItemToContainerAtSlot — FAIL: SourceContainer=%s (same=%s)"),
            *GetNameSafe(SourceContainer),
            (SourceContainer == TargetContainer) ? TEXT("true") : TEXT("false"));
        return false;
    }

    URiftItemInstance* DisplacedItem = TargetContainer->Slots.Entries[TargetSlotIndex].Item.Get();
    const int32 SourceSlotIndex = SourceContainer->GetSlotIndexOfItem(Item);

    // Validate dragged item is accepted by the target container.
    {
        FGameplayTagContainer ItemTags;
        Item->GetOwnedGameplayTags(ItemTags);
        if (!TargetContainer->ContainerDefinition || !TargetContainer->ContainerDefinition->AcceptsItemWithTags(ItemTags))
        {
            UE_LOG(LogTemp, Warning, TEXT("MoveItemToContainerAtSlot — FAIL: target container rejects item. Tags: %s"),
                *ItemTags.ToString());
            return false;
        }
    }

    // Per-slot tag requirement: if the target container has a tag requirement for this slot,
    // the incoming item must carry that tag (e.g. only chest pieces at the Chest slot index).
    if (TargetContainer->SlotTagRequirements.IsValidIndex(TargetSlotIndex)
        && TargetContainer->SlotTagRequirements[TargetSlotIndex].IsValid())
    {
        FGameplayTagContainer ItemTags;
        Item->GetOwnedGameplayTags(ItemTags);
        if (!ItemTags.HasTag(TargetContainer->SlotTagRequirements[TargetSlotIndex]))
        {
            UE_LOG(LogTemp, Warning, TEXT("MoveItemToContainerAtSlot — FAIL: item %s missing required slot tag %s for slot %d"),
                *GetNameSafe(Item),
                *TargetContainer->SlotTagRequirements[TargetSlotIndex].ToString(),
                TargetSlotIndex);
            return false;
        }
    }

    // If a swap is required, also validate the displaced item is accepted by the source container.
    if (IsValid(DisplacedItem))
    {
        FGameplayTagContainer DisplacedTags;
        DisplacedItem->GetOwnedGameplayTags(DisplacedTags);
        if (!SourceContainer->ContainerDefinition || !SourceContainer->ContainerDefinition->AcceptsItemWithTags(DisplacedTags))
        {
            UE_LOG(LogTemp, Warning, TEXT("MoveItemToContainerAtSlot — FAIL: source container rejects displaced item."));
            return false;
        }
    }

    // Perform the move/swap.
    SourceContainer->RefreshSlotEntry(SourceSlotIndex, DisplacedItem);
    TargetContainer->RefreshSlotEntry(TargetSlotIndex, Item);

    if (IsValid(DisplacedItem))
    {
        OnItemMoved.Broadcast(Item, SourceContainer, TargetContainer);
        OnItemMoved.Broadcast(DisplacedItem, TargetContainer, SourceContainer);
    }
    else
    {
        OnItemMoved.Broadcast(Item, SourceContainer, TargetContainer);
    }

    SaveInventory();
    return true;
}

void URiftInventoryComponent::Server_MoveItemToSlot_Implementation(FGameplayTag ContainerTag, int32 SourceSlotIndex, int32 TargetSlotIndex)
{
    URiftContainer* Container = FindContainerWithItemAtSlot(ContainerTag, SourceSlotIndex);
    if (!IsValid(Container)) { return; }

    URiftItemInstance* Item = Container->GetItemAtSlot(SourceSlotIndex);
    if (!IsValid(Item)) { return; }

    MoveItemToSlot(Item, TargetSlotIndex);
}

void URiftInventoryComponent::Server_MoveItemToContainerAtSlot_Implementation(FGameplayTag SourceContainerTag, int32 SourceSlotIndex, FGameplayTag TargetContainerTag, int32 TargetSlotIndex)
{
    URiftContainer* SourceContainer = FindContainerWithItemAtSlot(SourceContainerTag, SourceSlotIndex);
    if (!IsValid(SourceContainer))
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_MoveItemToContainerAtSlot — source container MISSING or no item at slot %d for tag %s"), SourceSlotIndex, *SourceContainerTag.ToString());
        return;
    }

    URiftItemInstance* Item = SourceContainer->GetItemAtSlot(SourceSlotIndex);
    if (!IsValid(Item))
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_MoveItemToContainerAtSlot — no item at source slot %d"), SourceSlotIndex);
        return;
    }

    URiftContainer* TargetContainer = nullptr;
    {
        FGameplayTagContainer ItemTags;
        Item->GetOwnedGameplayTags(ItemTags);
        for (const FRiftContainerEntry& Entry : Containers.Entries)
        {
            if (!IsValid(Entry.Container)) continue;
            if (Entry.Container->GetContainerTag() != TargetContainerTag) continue;
            if (!Entry.Container->ContainerDefinition
                || Entry.Container->ContainerDefinition->AcceptsItemWithTags(ItemTags))
            {
                TargetContainer = Entry.Container;
                break;
            }
        }
    }
    if (!IsValid(TargetContainer))
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_MoveItemToContainerAtSlot — no accepting container found for tag %s"), *TargetContainerTag.ToString());
        return;
    }

    MoveItemToContainerAtSlot(Item, TargetContainer, TargetSlotIndex);
}

void URiftInventoryComponent::Server_MoveItemToContainerAtSlotByObject_Implementation(URiftContainer* SourceContainer, int32 SourceSlotIndex, URiftContainer* TargetContainer, int32 TargetSlotIndex)
{
    // Validate that both containers actually belong to this inventory (server-side security check).
    bool bSourceOwned = false;
    bool bTargetOwned = false;
    for (const FRiftContainerEntry& Entry : Containers.Entries)
    {
        if (Entry.Container == SourceContainer) { bSourceOwned = true; }
        if (Entry.Container == TargetContainer)  { bTargetOwned = true; }
        if (bSourceOwned && bTargetOwned) { break; }
    }

    if (!bSourceOwned || !bTargetOwned)
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_MoveItemToContainerAtSlotByObject — container not owned by this inventory (src=%s tgt=%s)"),
            bSourceOwned ? TEXT("ok") : TEXT("FAIL"), bTargetOwned ? TEXT("ok") : TEXT("FAIL"));
        return;
    }

    URiftItemInstance* Item = IsValid(SourceContainer) ? SourceContainer->GetItemAtSlot(SourceSlotIndex) : nullptr;
    if (!IsValid(Item))
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_MoveItemToContainerAtSlotByObject — no item at source slot %d"), SourceSlotIndex);
        return;
    }

    MoveItemToContainerAtSlot(Item, TargetContainer, TargetSlotIndex);
}

void URiftInventoryComponent::Server_MoveItemToSlotByObject_Implementation(URiftContainer* Container, int32 SourceSlotIndex, int32 TargetSlotIndex)
{
    // Validate the container belongs to this inventory.
    bool bOwned = false;
    for (const FRiftContainerEntry& Entry : Containers.Entries)
    {
        if (Entry.Container == Container) { bOwned = true; break; }
    }
    if (!bOwned || !IsValid(Container)) { return; }

    URiftItemInstance* Item = Container->GetItemAtSlot(SourceSlotIndex);
    if (!IsValid(Item)) { return; }

    MoveItemToSlot(Item, TargetSlotIndex);
}

void URiftInventoryComponent::Server_MergeStacks_Implementation(FGameplayTag SourceContainerTag, int32 SourceSlotIndex, FGameplayTag TargetContainerTag, int32 TargetSlotIndex, int32 Quantity)
{
    URiftContainer* SourceContainer = FindContainerWithItemAtSlot(SourceContainerTag, SourceSlotIndex);
    URiftContainer* TargetContainer = FindContainerWithItemAtSlot(TargetContainerTag, TargetSlotIndex);
    if (!IsValid(SourceContainer) || !IsValid(TargetContainer)) { return; }

    URiftItemInstance* SourceItem = SourceContainer->GetItemAtSlot(SourceSlotIndex);
    URiftItemInstance* TargetItem = TargetContainer->GetItemAtSlot(TargetSlotIndex);
    if (!IsValid(SourceItem) || !IsValid(TargetItem)) { return; }

    URiftFragment_Stack* SourceStack = SourceItem->FindFragment<URiftFragment_Stack>();
    URiftFragment_Stack* TargetStack = TargetItem->FindFragment<URiftFragment_Stack>();

    if (!IsValid(SourceStack) || !IsValid(TargetStack) || !SourceStack->CanMergeWith(SourceItem, TargetItem))
    {
        return;
    }

    const int32 SourceQty = SourceStack->GetCurrentQuantity(SourceItem);
    const int32 QuantityToTransfer = (Quantity == -1) ? SourceQty : FMath::Min(Quantity, SourceQty);
    const int32 Overflow = TargetStack->AddQuantity(TargetItem, QuantityToTransfer);
    const int32 ActuallyTransferred = QuantityToTransfer - Overflow;

    if (ActuallyTransferred <= 0)
    {
        return;
    }

    if (ActuallyTransferred >= SourceQty)
    {
        RemoveItem(SourceItem);
    }
    else
    {
        SourceStack->RemoveQuantity(SourceItem, ActuallyTransferred);
    }
}

FGuid URiftInventoryComponent::SplitStack(URiftItemInstance* Item, int32 AmountToSplit)
{
    if (!IsValid(Item) || AmountToSplit <= 0)
    {
        return FGuid();
    }

    URiftFragment_Stack* StackFrag = Item->FindFragment<URiftFragment_Stack>();
    if (!IsValid(StackFrag))
    {
        return FGuid();
    }

    const int32 CurrentQuantity = StackFrag->GetCurrentQuantity(Item);
    if (AmountToSplit >= CurrentQuantity)
    {
        // Must leave at least 1 unit in the original stack.
        return FGuid();
    }

    URiftContainer* Container = GetContainerForItem(Item);
    if (!IsValid(Container) || !Container->HasAvailableSlot())
    {
        return FGuid();
    }

    // Remove the split amount from the source stack.
    StackFrag->RemoveQuantity(Item, AmountToSplit);

    // Create a new instance of the same definition and set its quantity.
    const FGuid NewItemId = AddItemToContainer(Item->GetDefinition(), Container, AmountToSplit);
    SaveInventory();

    return NewItemId;
}

URiftItemInstance* URiftInventoryComponent::GetItemById(const FGuid& ItemId) const
{
    for (const FRiftItemEntry& Entry : Items.Entries)
    {
        if (IsValid(Entry.Item) && Entry.Item->GetItemId() == ItemId)
        {
            return Entry.Item.Get();
        }
    }
    return nullptr;
}

URiftContainer* URiftInventoryComponent::GetContainerForItem(const URiftItemInstance* Item) const
{
    for (const FRiftContainerEntry& Entry : Containers.Entries)
    {
        if (IsValid(Entry.Container) && Entry.Container->HasItem(Item))
        {
            return Entry.Container.Get();
        }
    }
    return nullptr;
}

bool URiftInventoryComponent::HasItem(const URiftItemDefinition* Definition) const
{
    return GetItemsByDefinition(Definition).Num() > 0;
}

TArray<URiftItemInstance*> URiftInventoryComponent::GetItemsByDefinition(const URiftItemDefinition* Definition) const
{
    TArray<URiftItemInstance*> Result;
    for (const FRiftItemEntry& Entry : Items.Entries)
    {
        if (IsValid(Entry.Item) && Entry.Item->GetDefinition() == Definition)
        {
            Result.Add(Entry.Item.Get());
        }
    }
    return Result;
}

TArray<URiftItemInstance*> URiftInventoryComponent::GetAllItems() const
{
    // Items.Entries is a server-side stack-fill index and is not replicated.
    // On the owning client, items live in container slot caches — scan those instead.
    // On the server, both paths are equivalent; use containers for consistency.
    TArray<URiftItemInstance*> Result;
    for (const FRiftContainerEntry& ContEntry : Containers.Entries)
    {
        if (IsValid(ContEntry.Container))
        {
            for (const FRiftSlotEntry& SlotEntry : ContEntry.Container->Slots.Entries)
            {
                if (IsValid(SlotEntry.Item))
                {
                    Result.Add(SlotEntry.Item.Get());
                }
            }
        }
    }
    return Result;
}

int32 URiftInventoryComponent::CountItems() const
{
    return Items.Entries.Num();
}

void URiftInventoryComponent::BroadcastItemEvent(URiftItemInstance* Item, FGameplayTag EventTag)
{
    if (!GetOwner()->HasAuthority() || !IsValid(Item) || !EventTag.IsValid())
    {
        return;
    }

    // Route the event to any fragment that declared it in WatchedEventTags.
    // This is how equipment, durability, and custom fragments react to item
    // lifecycle changes without the inventory component knowing about them.
    const URiftItemDefinition* Definition = Item->GetDefinition();
    if (IsValid(Definition))
    {
        for (URiftItemFragment* Fragment : Definition->GetFragments())
        {
            if (IsValid(Fragment) && Fragment->IsRelevantEvent(EventTag))
            {
                Fragment->HandleItemEvent(Item, EventTag);
            }
        }
    }

    // Keep slot descriptor Quantity in sync whenever the stack size changes.
    if (EventTag == Tag_Rift_Event_Item_StackChanged)
    {
        RefreshSlotDescriptor(Item);
    }

    // Broadcast to external server-side listeners (Equipment component, etc.).
    OnItemEvent.Broadcast(Item, EventTag);
}

// ---------------------------------------------------------------------------
// Client reconstruction helpers
// ---------------------------------------------------------------------------

URiftItemInstance* URiftInventoryComponent::ReconstructItemInstance(URiftItemDefinition* Definition, int32 Quantity)
{
    if (!IsValid(Definition))
    {
        return nullptr;
    }

    URiftItemInstance* NewItem = NewObject<URiftItemInstance>(this);
    NewItem->SetDefinition(Definition);

    // Enable local reconstruction mode so SaveStateForFragment bypasses HasAuthority().
    NewItem->bReconstructingLocally = true;

    // Initialize fragment states (e.g. FRiftStackState with CurrentQuantity = 1).
    NewItem->InitializeFragmentStatesLocally();

    // Override stack quantity with the replicated value if it differs from the default.
    if (Quantity > 1 && NewItem->FindFragment<URiftFragment_Stack>())
    {
        TInstancedStruct<FRiftFragmentState> NewState = TInstancedStruct<FRiftFragmentState>::Make<FRiftStackState>();
        FRiftStackState& StackState = NewState.GetMutable<FRiftStackState>();
        StackState.CurrentQuantity = Quantity;
        StackState.LastQuantity    = Quantity;
        NewItem->SaveStateForFragment(URiftFragment_Stack::StaticClass(), NewState);
    }

    NewItem->bReconstructingLocally = false;

    return NewItem;
}

void URiftInventoryComponent::EvictItemFromCache(URiftItemInstance* Item)
{
    // The item instance is referenced solely by the slot entry's Item pointer.
    // The caller clears that pointer; nothing else references it on the client.
    // This is a no-op hook for future client-side cache management if needed.
}

void URiftInventoryComponent::UpdateItemInstanceQuantity(URiftItemInstance* Item, int32 NewQuantity)
{
    if (!IsValid(Item) || NewQuantity <= 0)
    {
        return;
    }

    URiftFragment_Stack* StackFrag = Item->FindFragment<URiftFragment_Stack>();
    if (!StackFrag)
    {
        return;
    }

    // Directly update the stack state to match the server-replicated quantity.
    // We use bReconstructingLocally so SaveStateForFragment works on the client.
    Item->bReconstructingLocally = true;

    TInstancedStruct<FRiftFragmentState> NewState = TInstancedStruct<FRiftFragmentState>::Make<FRiftStackState>();
    FRiftStackState& StackState = NewState.GetMutable<FRiftStackState>();
    StackState.CurrentQuantity = NewQuantity;
    StackState.LastQuantity    = StackFrag->GetCurrentQuantity(Item);  // preserve old for delta
    Item->SaveStateForFragment(URiftFragment_Stack::StaticClass(), NewState);

    Item->bReconstructingLocally = false;

    // Notify client-side listeners (ViewModels, UI) that the stack quantity changed.
    OnItemEvent.Broadcast(Item, Tag_Rift_Event_Item_StackChanged);
}

void URiftInventoryComponent::RefreshSlotDescriptor(URiftItemInstance* Item)
{
    if (!IsValid(Item) || !GetOwner()->HasAuthority())
    {
        return;
    }

    for (const FRiftContainerEntry& ContEntry : Containers.Entries)
    {
        if (!IsValid(ContEntry.Container))
        {
            continue;
        }

        const int32 SlotIndex = ContEntry.Container->GetSlotIndexOfItem(Item);
        if (SlotIndex == INDEX_NONE)
        {
            continue;
        }

        FRiftSlotEntry& SlotEntry = ContEntry.Container->Slots.Entries[SlotIndex];
        SlotEntry.Quantity = Item->GetCurrentQuantity();
        ContEntry.Container->Slots.MarkItemDirty(SlotEntry);
        return;
    }
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

URiftItemInstance* URiftInventoryComponent::CreateItemInstance(URiftItemDefinition* Definition)
{
    URiftItemInstance* NewItem = NewObject<URiftItemInstance>(this);
    NewItem->SetDefinition(Definition);
    NewItem->SetItemId(FGuid::NewGuid());
    // Items are no longer registered as network subobjects. Clients reconstruct item
    // state locally from the slot descriptor (ItemDefinition + Quantity) replicated
    // via FRiftSlotList. The FGuid is retained for server-side persistence only.
    return NewItem;
}

URiftContainer* URiftInventoryComponent::CreateContainer(URiftContainerDefinition* Definition)
{
    URiftContainer* NewContainer = NewObject<URiftContainer>(this);
    NewContainer->SetDefinition(Definition);
    NewContainer->SetContainerId(FGuid::NewGuid());
    FRiftContainerEntry Entry;
    Entry.Container = NewContainer;
    Containers.Entries.Add(Entry);
    Containers.MarkItemDirty(Containers.Entries.Last());
    AddReplicatedSubObject(NewContainer, COND_OwnerOnly);
    OnContainerAdded.Broadcast(NewContainer);
    return NewContainer;
}

URiftContainer* URiftInventoryComponent::FindBestContainerForItem(const URiftItemInstance* Item) const
{
    for (const FRiftContainerEntry& Entry : Containers.Entries)
    {
        URiftContainer* Container = Entry.Container.Get();
        if (IsValid(Container) && Container->CanAcceptItem(Item))
        {
            return Container;
        }
    }
    return nullptr;
}

URiftContainer* URiftInventoryComponent::FindContainerWithItemAtSlot(FGameplayTag ContainerTag, int32 SlotIndex) const
{
    for (const FRiftContainerEntry& Entry : Containers.Entries)
    {
        if (!IsValid(Entry.Container)) { continue; }
        if (Entry.Container->GetContainerTag() != ContainerTag) { continue; }
        if (IsValid(Entry.Container->GetItemAtSlot(SlotIndex)))
        {
            return Entry.Container.Get();
        }
    }
    return nullptr;
}

int32 URiftInventoryComponent::FinalizeNewItem(URiftItemInstance* NewItem, URiftContainer* Container, int32 RemainingQuantity)
{
    NewItem->InitializeFragmentStates();

    URiftFragment_Stack* StackFrag = NewItem->FindFragment<URiftFragment_Stack>();
    if (StackFrag && RemainingQuantity > 1)
    {
        RemainingQuantity = StackFrag->AddQuantity(NewItem, RemainingQuantity - 1);
    }
    else
    {
        RemainingQuantity--;
    }

    NewItem->ActivateFragments();
    RefreshSlotDescriptor(NewItem);
    BroadcastItemEvent(NewItem, Tag_Rift_Event_Item_Added);
    OnItemAdded.Broadcast(NewItem, Container);

    return RemainingQuantity;
}

// ---------------------------------------------------------------------------
// Drop / Delete Server RPCs
// ---------------------------------------------------------------------------

static ARiftPickup* FindNearbyPickup_Internal(UWorld* World, const FVector& Origin, float RadiusSq)
{
    if (!World) return nullptr;

    for (TActorIterator<ARiftPickup> It(World); It; ++It)
    {
        ARiftPickup* Pickup = *It;
        if (!IsValid(Pickup) || !Pickup->GetDropInventory()) continue;
        if (FVector::DistSquared(Pickup->GetActorLocation(), Origin) <= RadiusSq)
        {
            return Pickup;
        }
    }
    return nullptr;
}

static FVector GetDropLocation_Internal(AActor* Actor)
{
    if (!IsValid(Actor)) return FVector::ZeroVector;

    const FVector Loc       = Actor->GetActorLocation();
    const FVector Forward   = Actor->GetActorForwardVector();
    const float   HalfHeight = Actor->GetSimpleCollisionHalfHeight();

    return Loc + (Forward * 60.f) - FVector(0.f, 0.f, HalfHeight);
}

void URiftInventoryComponent::Server_DropItem_Implementation(FGameplayTag ContainerTag, int32 SlotIndex, float ConsolidationRadius, TSubclassOf<ARiftPickup> PickupClass)
{
    URiftContainer* Container = FindContainerWithItemAtSlot(ContainerTag, SlotIndex);
    if (!IsValid(Container))
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_DropItem — no container with tag %s holding an item at slot %d"), *ContainerTag.ToString(), SlotIndex);
        return;
    }
    Server_DropItemByObject(Container, SlotIndex, ConsolidationRadius, PickupClass);
}

void URiftInventoryComponent::Server_DropItemByObject_Implementation(URiftContainer* Container, int32 SlotIndex, float ConsolidationRadius, TSubclassOf<ARiftPickup> PickupClass)
{
    if (!IsValid(Container)) { return; }

    URiftItemInstance* Item = Container->GetItemAtSlot(SlotIndex);
    if (!IsValid(Item))
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_DropItemByObject — no item at slot %d in container %s"), SlotIndex, *Container->GetContainerTag().ToString());
        return;
    }

    URiftFragment_Drop* DropFrag = Item->FindFragment<URiftFragment_Drop>();

    if (!DropFrag || !DropFrag->IsDroppable())
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_DropItemByObject — item '%s' is not droppable (no Drop fragment or bDroppable=false)"), *GetNameSafe(Item->GetDefinition()));
        return;
    }

    APawn* OwningPawn = nullptr;
    if (APlayerState* PS = Cast<APlayerState>(GetOwner()))
    {
        OwningPawn = PS->GetPawn();
    }
    if (!OwningPawn)
    {
        OwningPawn = Cast<APawn>(GetOwner());
    }

    int32 Qty = 1;
    if (URiftFragment_Stack* StackFrag = Item->FindFragment<URiftFragment_Stack>())
    {
        Qty = StackFrag->GetCurrentQuantity(Item);
    }

    URiftItemDefinition* Def = Item->GetDefinition();

    RemoveItem(Item);

    UWorld* World = GetWorld();
    if (!World) return;

    const FVector Origin = OwningPawn ? OwningPawn->GetActorLocation() : FVector::ZeroVector;

    ARiftPickup* Pickup = (ConsolidationRadius > 0.f)
        ? FindNearbyPickup_Internal(World, Origin, ConsolidationRadius * ConsolidationRadius)
        : nullptr;

    if (!Pickup)
    {
        TSubclassOf<ARiftPickup> SpawnClass = PickupClass
            ? PickupClass
            : TSubclassOf<ARiftPickup>(ARiftPickup::StaticClass());

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        const FVector DropLoc = OwningPawn
            ? GetDropLocation_Internal(OwningPawn)
            : FVector::ZeroVector;

        Pickup = World->SpawnActor<ARiftPickup>(SpawnClass, DropLoc, FRotator::ZeroRotator, Params);
    }

    if (!IsValid(Pickup))
    {
        return;
    }

    Pickup->AddDroppedItem(Def, Qty);

    if (UStaticMesh* DropMesh = DropFrag->GetDropMesh())
    {
        Pickup->SetDropMesh(DropMesh);
    }
}

void URiftInventoryComponent::Server_DeleteItem_Implementation(FGameplayTag ContainerTag, int32 SlotIndex)
{
    URiftContainer* Container = FindContainerWithItemAtSlot(ContainerTag, SlotIndex);
    if (!IsValid(Container))
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_DeleteItem — no container with tag %s holding an item at slot %d"), *ContainerTag.ToString(), SlotIndex);
        return;
    }
    Server_DeleteItemByObject(Container, SlotIndex);
}

void URiftInventoryComponent::Server_DeleteItemByObject_Implementation(URiftContainer* Container, int32 SlotIndex)
{
    if (!IsValid(Container)) { return; }

    URiftItemInstance* Item = Container->GetItemAtSlot(SlotIndex);
    if (!IsValid(Item))
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_DeleteItemByObject — no item at slot %d"), SlotIndex);
        return;
    }

    URiftFragment_Drop* DropFrag = Item->FindFragment<URiftFragment_Drop>();

    if (DropFrag && !DropFrag->IsDeletable())
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_DeleteItemByObject — '%s' is not deletable."),
            *GetNameSafe(Item->GetDefinition()));
        return;
    }

    RemoveItem(Item);
}
