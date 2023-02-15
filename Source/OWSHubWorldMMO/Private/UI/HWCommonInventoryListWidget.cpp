// Copyright 2022 Sabre Dart Studios


#include "./UI/HWCommonInventoryListWidget.h"
#include "./Player/HWPlayerController.h"
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