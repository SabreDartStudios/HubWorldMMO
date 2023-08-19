// Copyright 2023 Sabre Dart Studios


#include "UI/HWMobNameplateWidget.h"
#include "Components/TileView.h"
#include "../OWSHubWorldMMO.h"

void UHWMobNameplateWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshSource();
}

void UHWMobNameplateWidget::RefreshSource()
{
	if (OwningHWGASCharacter)
	{
		CombatStateTileView->SetListItems(OwningHWGASCharacter->CombatStateDisplayItems);
	}
}