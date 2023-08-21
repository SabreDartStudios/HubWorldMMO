// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HWGASCharacter.h"
#include "./UI/HWMobNameplateWidgetComponent.h"
#include "HWGASMobCharacter.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API AHWGASMobCharacter : public AHWGASCharacter
{
	GENERATED_BODY()

public:
	AHWGASMobCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nameplate")
		UHWMobNameplateWidgetComponent* MobNameplateWidgetComponent;

protected:
	virtual void ReloadUIForCombatStateDisplayItems() override;
};
