// Copyright 2023 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "OWSGameMode.h"
#include "HWGameMode.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API AHWGameMode : public AOWSGameMode
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, Category = "Combat States")
	TSubclassOf<UGameplayEffect> ApplyWetGameplayEffect;
};
