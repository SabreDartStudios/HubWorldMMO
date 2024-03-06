// Copyright 2023 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/HWGameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "HWGameplayAbility_Combo.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWGameplayAbility_Combo : public UHWGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combo")
		int32 CurrentComboIndex;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combo")
		FGameplayTag NextComboTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combo")
		float BranchingPointTime;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combo")
		float DamageTimeStamp;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Combo")
		float AnimationDuration;

	UFUNCTION(BlueprintImplementableEvent)
		void AbilityStart();

	UFUNCTION(BlueprintImplementableEvent)
		void AbilityApplyDamage();

protected:
	bool bAdvanceToNextCombo;

	UFUNCTION()
		void OnInputRelease(float TimeHeld);

	UFUNCTION()
		void OnInputPress(float TimeWaited);

	UFUNCTION()
		void OnBranchingPointReached();

	UFUNCTION()
		void OnDamageTimeStampReached();

	UFUNCTION()
		void OnAnimationEnded();

	UPROPERTY(Transient)
		TObjectPtr<UAbilityTask_WaitInputRelease> AbilityTaskInputRelease;

	UPROPERTY(Transient)
		TObjectPtr<UAbilityTask_WaitInputPress> AbilityTaskInputPress;

	UPROPERTY(Transient)
		TObjectPtr<UAbilityTask_WaitDelay> AbilityTaskWaitDelayBranchingPoint;

	UPROPERTY(Transient)
		TObjectPtr<UAbilityTask_WaitDelay> AbilityTaskWaitDelayDamageTimeStamp;

	UPROPERTY(Transient)
		TObjectPtr<UAbilityTask_WaitDelay> AbilityTaskWaitDelayAnimationDuration;
};
