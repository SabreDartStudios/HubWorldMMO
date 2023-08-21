// Copyright 2023 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "./Character/HwGASCharacter.h"
#include "HWMobNameplateWidget.generated.h"



/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWMobNameplateWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
		AHWGASCharacter* OwningHWGASCharacter;

	UFUNCTION(BlueprintCallable, Category = "Combat State")
	void RefreshSource();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
		class UTileView* CombatStateTileView;
};
