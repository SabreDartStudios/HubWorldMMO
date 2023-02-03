// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_Radius.h"
#include "HWATA_ConeTrace.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API AHWATA_ConeTrace : public AGameplayAbilityTargetActor_Radius
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Settings)
		float HalfAngle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = Settings)
		FVector ForwardVector;

	void ConfirmTargetingAndContinue() override;
	
protected:
	TArray< TWeakObjectPtr<AActor>> PerformOverlap(const FVector& Origin);

};
