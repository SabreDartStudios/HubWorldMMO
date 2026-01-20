// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "UObject/UnrealType.h"
//#include "Engine/NetSerialization.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "HWInventoryComponent.generated.h"

//Row definition for the InventoryLibrary Data Table
USTRUCT()
struct FInventoryItemTypes : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FInventoryItemTypes() 
	{
		ItemName = "";
		ItemDescription = "";
		bCanStack = false;
		StackSize = 0;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FString ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TSoftObjectPtr<UTexture2D> ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		bool bCanStack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 StackSize;
};

//Row definition for the combined runtime and setup data for displaying the inventory in the UI
USTRUCT(BlueprintType, Blueprintable)
struct FHWInventoryDisplayItem
{
	GENERATED_USTRUCT_BODY()

	FHWInventoryDisplayItem()
	{
		ItemGUID = FGuid();
		ItemTypeID = 0;
		Quantity = 1;
		InSlotNumber = 1;
		NumberOfUsesLeft = 0;
		Condition = 0;
		CustomJSON = "";
		ItemName = FText::GetEmpty();
		ItemDescription = FText::GetEmpty();
		bCanStack = false;
		StackSize = 0;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		FGuid ItemGUID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 ItemTypeID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 Quantity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 InSlotNumber;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 NumberOfUsesLeft;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 Condition;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		FString CustomJSON;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FText ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FText ItemDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TSoftObjectPtr<UTexture2D> ItemIcon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		bool bCanStack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 StackSize;

};

//UObject to contain the inventory row struct (this is required by the UI system)
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OWSHUBWORLDMMO_API UHWInventoryDisplayItemObject : public UObject
{
	GENERATED_BODY()

public:
	FHWInventoryDisplayItem Data;

};

//Runtime row definition for replicated array using FastArraySerializer
USTRUCT(BlueprintType, Blueprintable)
struct FHWInventoryItem : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	friend struct FHWInventoryMaster;

	FHWInventoryItem()
	{
		ItemGUID = FGuid();
		ItemTypeID = 0;
		Quantity = 1;
		InSlotNumber = 1;
		NumberOfUsesLeft = 0;
		Condition = 0;
		FlatAttack = 0;
		FlatDefense = 0;
		FlatHP = 0;
		AttackPercent = 0.f;
		DefensePercent = 0.f;
		HPPercent = 0.f;
		CritRate = 0.f;
		CritDamage = 0.f;
	}

	/*
	void PreReplicatedRemove(const struct FHWInventoryMaster& InArray);
	void PostReplicatedAdd(const struct FHWInventoryMaster& InArray);
	void PostReplicatedChange(const struct FHWInventoryMaster& InArray);
	*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		FGuid ItemGUID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 ItemTypeID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 Quantity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 InSlotNumber;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 NumberOfUsesLeft;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 Condition;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 FlatAttack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 FlatDefense;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		int32 FlatHP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		float AttackPercent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		float DefensePercent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		float HPPercent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		float CritRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		float CritDamage;

};

//Runtime replicated inventory data 
USTRUCT(BlueprintType, Blueprintable)
struct FHWInventoryMaster : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	friend struct FHWInventoryItem;

	FHWInventoryMaster()
	{

	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		TArray<FHWInventoryItem> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FHWInventoryItem>(Items, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FHWInventoryMaster> : public TStructOpsTypeTraitsBase2<FHWInventoryMaster> //TStructOpsTypeTraitsBase
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OWSHUBWORLDMMO_API UHWInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHWInventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Inventory, Category = "Inventory")
		FHWInventoryMaster Inventory;

	UFUNCTION()
		virtual void OnRep_Inventory();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FString InventoryName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int32 InventorySize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		UDataTable* ItemLibrary;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		FGuid GenerateInventoryItem();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void AddItemToInventory(int32 ItemTypeID, int32 Quantity, int32 NumberOfUsesLeft, int32 Condition, FString CustomJSON);

		void AddItemToInventory(const FHWInventoryItem& ItemToAdd);

		UFUNCTION(BlueprintCallable, Category = "Inventory")
			void PersistInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void LoadInventoryFromJSON(FString InventoryJSON);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		FString SerializeInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		const FHWInventoryItem& FindItemInInventoryWithRoomInStack(int32 ItemTypeID, int32 SpaceNeeded);

	//UI Display
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
		TArray<UHWInventoryDisplayItemObject*> InventoryDisplayItems;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void AddDisplayItem(UHWInventoryDisplayItemObject* ItemToAdd);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void RemoveDisplayItem(UHWInventoryDisplayItemObject* ItemToRemove);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ReloadDisplayItems();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FHWInventoryItem NullInventoryItem;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//int32* GetReplicationList(uint8* Recent, FPropertyRetirement* Retire, int32* Ptr, UPackageMap* Map, UActorChannel* Channel, FReplicationFlags RepFlags);
		
};


