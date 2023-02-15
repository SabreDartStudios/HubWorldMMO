// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "HWGameplayAbility.h"
#include "HWAT_WaitMultiTraceForTargets.h"
#include "HWGameplayAbility_TraceSteps.generated.h"



/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWGameplayAbility_TraceSteps : public UHWGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Collision")
		TArray<FTraceStep> TraceSteps;
};
