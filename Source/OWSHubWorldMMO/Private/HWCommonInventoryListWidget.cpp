// Copyright 2022 Sabre Dart Studios


#include "HWCommonInventoryListWidget.h"
#include "HWPlayerController.h"
#include "CommonListView.h"

void UHWCommonInventoryListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//AHWPlayerController* HWPlayerController = Cast<AHWPlayerController>(GetOwningPlayer());

	//SetInventoryComponent(HWPlayerController->InventoryComponentForInventoryUI);

	if (InventoryComponent && InventoryListView)
	{
		InventoryComponent->ReloadDisplayItems();
		InventoryListView->SetListItems(InventoryComponent->InventoryDisplayItems);
	}
}

void UHWCommonInventoryListWidget::SetInventoryComponent(UHWInventoryComponent* NewInventoryComponent)
{
	InventoryComponent = NewInventoryComponent;
}