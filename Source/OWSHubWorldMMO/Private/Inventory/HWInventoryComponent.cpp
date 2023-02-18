// Copyright 2022 Sabre Dart Studios


#include "./Inventory/HWInventoryComponent.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Net/UnrealNetwork.h"
#include "../OWSHubWorldMMO.h"

// Sets default values for this component's properties
UHWInventoryComponent::UHWInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	NullInventoryItem.ItemGUID = FGuid(0, 0, 0, 0);

}


// Called when the game starts
void UHWInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHWInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


FGuid UHWInventoryComponent::GenerateInventoryItem()
{
	return FGuid::NewGuid();
}

void UHWInventoryComponent::AddItemToInventory(int32 ItemTypeID, int32 Quantity, int32 NumberOfUsesLeft, int32 Condition, FString CustomJSON)
{
	FGuid ItemGUID = GenerateInventoryItem();

	FHWInventoryItem ItemToAdd;
	ItemToAdd.ItemGUID = ItemGUID;
	ItemToAdd.ItemTypeID = ItemTypeID;
	ItemToAdd.Quantity = Quantity;
	ItemToAdd.NumberOfUsesLeft = NumberOfUsesLeft;
	ItemToAdd.Condition = Condition;
	ItemToAdd.CustomJSON = CustomJSON;

	ItemToAdd.InSlotNumber = 0;

	FHWInventoryItem& ReferenceToAddedItem = Inventory.Items.Add_GetRef(ItemToAdd);
	Inventory.MarkItemDirty(ReferenceToAddedItem);
}

void UHWInventoryComponent::LoadInventoryFromJSON(FString InventoryJSON)
{
	FJsonObjectConverter::JsonObjectStringToUStruct(InventoryJSON, &Inventory);
}

FString UHWInventoryComponent::SerializeInventory()
{
	FString SerializedInventory = "";
	if (FJsonObjectConverter::UStructToJsonObjectString(Inventory, SerializedInventory))
	{
		return SerializedInventory;
	}
	return "";
}

const FHWInventoryItem& UHWInventoryComponent::FindItemInInventoryWithRoomInStack(int32 ItemTypeID, int32 SpaceNeeded)
{
	if (ItemLibrary)
	{
		FInventoryItemTypes* FoundItemType = ItemLibrary->FindRow<FInventoryItemTypes>(FName(FString::FromInt(ItemTypeID)), "");
		if (FoundItemType)
		{
			if (FoundItemType->bCanStack && FoundItemType->StackSize > SpaceNeeded)
			{
				//Loop through inventory items and find the first stack with available space
				for (auto& InventoryItem : Inventory.Items)
				{
					if (InventoryItem.ItemTypeID == ItemTypeID)
					{
						if (FoundItemType->StackSize - InventoryItem.Quantity >= SpaceNeeded)
						{
							return InventoryItem;
						}
					}
				}
			}
		}
	}

	return NullInventoryItem;
}

void UHWInventoryComponent::OnRep_Inventory()
{
	ReloadDisplayItems();
}

void UHWInventoryComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Fast Arrays don't use push model, but there's no harm in marking them with it.
	// The flag will just be ignored.

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_OwnerOnly;

	DOREPLIFETIME_WITH_PARAMS_FAST(UHWInventoryComponent, Inventory, Params);
}


//UI Display
void UHWInventoryComponent::AddDisplayItem(UHWInventoryDisplayItemObject* ItemToAdd)
{
	InventoryDisplayItems.Add(ItemToAdd);
}

void UHWInventoryComponent::RemoveDisplayItem(UHWInventoryDisplayItemObject* ItemToAdd)
{
	InventoryDisplayItems.Remove(ItemToAdd);
}

void UHWInventoryComponent::ReloadDisplayItems()
{
	//Only allow this to run on the client side
	if (GetOwner()->HasAuthority())
	{
		return;
	}

	UE_LOG(OWSHubWorldMMO, Verbose, TEXT("UHWInventoryComponent - ReloadDisplayItems Started on the Client Side"));

	int32 Slack = Inventory.Items.Num();
	InventoryDisplayItems.Empty(Slack);

	for (auto& InventoryItem : Inventory.Items)
	{
		UHWInventoryDisplayItemObject* ItemToAdd = NewObject<UHWInventoryDisplayItemObject>(this, UHWInventoryDisplayItemObject::StaticClass());
		FInventoryItemTypes* FoundItemType = ItemLibrary->FindRow<FInventoryItemTypes>(FName(FString::FromInt(InventoryItem.ItemTypeID)), "");
		if (FoundItemType)
		{
			//Add details from Runtime inventory data (InventoryItem)
			ItemToAdd->Data.ItemGUID = InventoryItem.ItemGUID;
			ItemToAdd->Data.Condition = InventoryItem.Condition;
			ItemToAdd->Data.CustomJSON = InventoryItem.CustomJSON;
			ItemToAdd->Data.InSlotNumber = InventoryItem.InSlotNumber;
			ItemToAdd->Data.ItemTypeID = InventoryItem.ItemTypeID;
			ItemToAdd->Data.NumberOfUsesLeft = InventoryItem.NumberOfUsesLeft;
			ItemToAdd->Data.Quantity = InventoryItem.Quantity;

			//Add details from Setup inventory data (FoundItem from ItemLibrary Data Table)
			ItemToAdd->Data.ItemName = FText::FromString(FoundItemType->ItemName);
			ItemToAdd->Data.ItemDescription = FText::FromString(FoundItemType->ItemDescription);
			ItemToAdd->Data.ItemIcon = FoundItemType->ItemIcon;
			ItemToAdd->Data.bCanStack = FoundItemType->bCanStack;
			ItemToAdd->Data.StackSize = FoundItemType->StackSize;

			//Add the item to the array
			InventoryDisplayItems.Add(ItemToAdd);
		}
		else
		{
			UE_LOG(OWSHubWorldMMO, Error, TEXT("UHWInventoryComponent - Item Not Found in Item Library - ItemTypeID: %d"), InventoryItem.ItemTypeID);
		}
	}
}