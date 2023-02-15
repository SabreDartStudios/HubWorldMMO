// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "./Inventory/HWInventoryComponent.h"
#include "HWInventoryListWidget.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWInventoryListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		UHWInventoryComponent* InventoryComponent;
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
		class UListView* InventoryListView;
};
