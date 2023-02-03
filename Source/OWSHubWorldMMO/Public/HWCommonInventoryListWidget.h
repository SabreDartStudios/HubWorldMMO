// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "HWInventoryComponent.h"
#include "HWCommonInventoryListWidget.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWCommonInventoryListWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		UHWInventoryComponent* InventoryComponent;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetInventoryComponent(UHWInventoryComponent* NewInventoryComponent);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
		class UCommonListView* InventoryListView;
	
};
