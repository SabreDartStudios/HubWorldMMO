// Copyright 2023 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "HWCombatStateIconEntry.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class OWSHUBWORLDMMO_API UHWCombatStateIconEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	// Begin IUserObjectListEntry Interface Implementations
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	// End IUserObjectListEntry Interface Implementations

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* IconImage;
};
