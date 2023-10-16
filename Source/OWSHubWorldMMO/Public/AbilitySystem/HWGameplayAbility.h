// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "./Character/HWGASCharacter.h"
#include "HWGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EHWAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// This ability is always active.
	Passive
};


UENUM(BlueprintType)
enum class EHWAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,

	MAX	UMETA(Hidden)
};

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UHWGameplayAbility();

	AHWGASCharacter* GetHWAvatarActor();

	UFUNCTION(BlueprintCallable, Category = "Combat Data")
		float GetFloatValueFromCombatData(FString FloatCombatValueName);

	bool GetEndAbilityThisFrame() const { return bEndAbilityThisFrame; };
	EHWAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; };
	EHWAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; };
	bool GetShouldReplicateInputPressedEventsWhenActive() const { return bShouldReplicateInputPressedEventsWhenActive; };
	bool GetShouldReplicateInputReleasedEventsWhenActive() const { return bShouldReplicateInputReleasedEventsWhenActive; };

	void EndAbilityThisFrame();

protected:

	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const override;

	//Set the name of the ability for use with Combat Data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HW|Ability")
		FString AbilityName;

	//Set this to true to use end the ability in the same frame.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HW|Ability Activation")
		bool bEndAbilityThisFrame;

	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HW|Ability Activation")
		EHWAbilityActivationPolicy ActivationPolicy;

	// Defines the relationship between this ability activating and other abilities activating.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HW|Ability Activation")
		EHWAbilityActivationGroup ActivationGroup;

	// Should we replicate input pressed events to the server when this ability is already active.  This is required to use certain Input related Ability Tasks.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HW|Input Replication")
		bool bShouldReplicateInputPressedEventsWhenActive;

	// Should we replicate input released events to the server when this ability is already active.  This is required to use certain Input related Ability Tasks.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HW|Input Replication")
		bool bShouldReplicateInputReleasedEventsWhenActive;
};
