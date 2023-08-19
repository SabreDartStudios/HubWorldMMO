// Copyright 2023 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HWMobNameplateWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWMobNameplateWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Combat State")
		void RefreshLinkToParent();
};
