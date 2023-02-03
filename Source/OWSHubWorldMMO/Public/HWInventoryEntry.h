// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "HWInventoryEntry.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class OWSHUBWORLDMMO_API UHWInventoryEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	// Begin IUserObjectListEntry Interface Implementations
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	// End IUserObjectListEntry Interface Implementations

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* NameLabel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* IconImage;
};
