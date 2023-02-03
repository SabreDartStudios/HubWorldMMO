// Copyright 2022 Sabre Dart Studios


#include "HWInventoryEntry.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "HWInventoryComponent.h"

void UHWInventoryEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UHWInventoryDisplayItemObject* Item = Cast<UHWInventoryDisplayItemObject>(ListItemObject);
	NameLabel->SetText(Item->Data.ItemName);
	IconImage->SetBrushFromTexture(Item->Data.ItemIcon.Get());
}
