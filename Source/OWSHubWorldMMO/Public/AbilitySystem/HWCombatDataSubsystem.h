// Copyright 2023 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HWCombatDataSubsystem.generated.h"

USTRUCT(Blueprintable)
struct FCombatAbility
{
	GENERATED_USTRUCT_BODY()

public:
	FCombatAbility()
	{
		AbilityName = "";
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Data")
		FString AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Data")
		TMap<FString, float> FloatCombatValues;
};

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWCombatDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	UPROPERTY()
		TMap<FString, FCombatAbility> CombatAbilities;
	
public:

	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	UFUNCTION(BlueprintCallable, Category = "Combat Data")
		float GetFloatValue(FString AbilityName, FString FloatCombatValueName);
};
