#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitLanded.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FApplyRootMotionJumpForceDelegate);

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UAbilityTask_WaitLanded : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
		FApplyRootMotionJumpForceDelegate OnLanded;
	
	UFUNCTION()
		void OnLandedCallback(const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAbilityTask_WaitLanded* WaitLanded(UGameplayAbility* OwningAbility);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityIsEnding) override;

protected:

	void TriggerLanded();

	bool bHasLanded;
};
