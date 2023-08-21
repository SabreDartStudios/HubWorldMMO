// Copyright 2023 Sabre Dart Studios


#include "UI/HWCombatStateIconEntry.h"
#include "Components/Image.h"
#include "./UI/HWMobNameplateWidget.h"


void UHWCombatStateIconEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UHWCombatStateDisplayItemObject* Item = Cast<UHWCombatStateDisplayItemObject>(ListItemObject);
		IconImage->SetBrushFromTexture(Item->Data.ItemIcon.LoadSynchronous());
}