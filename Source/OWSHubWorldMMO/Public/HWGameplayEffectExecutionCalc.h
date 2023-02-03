// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "HWGameplayEffectExecutionCalc.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWGameplayEffectExecutionCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	UHWGameplayEffectExecutionCalc();

	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
