// Copyright 2022 Sabre Dart Studios


#include "./UI/HWInventoryListWidget.h"
#include "Components/ListView.h"

void UHWInventoryListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InventoryComponent && InventoryComponent->InventoryDisplayItems.Num() > 0)
	{
		InventoryListView->SetListItems(InventoryComponent->InventoryDisplayItems);
	}
}