// Copyright 2023 Sabre Dart Studios


#include "UI/HWMobNameplateWidgetComponent.h"
#include "UI/HWMobNameplateWidget.h"

void UHWMobNameplateWidgetComponent::RefreshLinkToParent()
{
	UHWMobNameplateWidget* HWMobNameplateWidget = Cast<UHWMobNameplateWidget>(GetWidget());

	if (HWMobNameplateWidget)
	{
		AHWGASCharacter* HWGASCharacter = Cast<AHWGASCharacter>(GetOwner());

		if (HWGASCharacter)
		{
			HWMobNameplateWidget->OwningHWGASCharacter = HWGASCharacter;
		}
	}
}