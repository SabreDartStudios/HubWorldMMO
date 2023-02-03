// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "HWAT_WaitMultiTraceForTargets.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitMultiTraceForTargetsDelegate, const FGameplayAbilityTargetDataHandle&, Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitMultiTraceForTargetsFinishedDelegate);

USTRUCT(BlueprintType, Blueprintable)
struct FTraceStep
{
	GENERATED_USTRUCT_BODY()

	FTraceStep()
	{
		StartTimeInSeconds = 0.f;
		CollisionShape = 0;
		Radius = 100.f;
		bActivated = false;
		bDisabled = false;
		bDebug = false;
	}

	/** How many seconds to wait to activate this instant one-frame trace.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
		float StartTimeInSeconds;
	/** The associated Gameplay Effect.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
		TSubclassOf<UGameplayEffect> AssociatedGameplayEffect;
	/**
	0 - Sphere
	1 - Capsule
	2 - Box
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
		int32 CollisionShape;
	/** The radius for Sphere's and Capsule's.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
		float Radius;
	/** The offset of the trace from actor origin.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
		FVector OffsetFromOrigin;
	/** Can this trace hit the attacker?*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
		bool bCanHitSelf;
	/** For debugging.  Disables this step without having to remove it from the array.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
		bool bDisabled;
	/** For debugging.  Show debug trace on screen.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
		bool bDebug;

	bool bActivated;
};

/**
 * 
 */
UCLASS(notplaceable)
class OWSHUBWORLDMMO_API UHWAT_WaitMultiTraceForTargets : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true", HideSpawnParms = "Instigator"), Category = "Ability|Tasks")
		static UHWAT_WaitMultiTraceForTargets* WaitMultiTraceForTargets(UGameplayAbility* OwningAbility, FName TaskInstanceName, TArray<FTraceStep> TraceSteps, bool CombineTargets);

	UPROPERTY(BlueprintAssignable)
		FWaitMultiTraceForTargetsDelegate ValidData;

	UPROPERTY(BlueprintAssignable)
		FWaitMultiTraceForTargetsFinishedDelegate Finished;

	UFUNCTION()
		void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data);

	/** List of traces to perform.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
		TArray<FTraceStep> TraceSteps;

	/** When set to true, combine all targets and only hit each one once.  When set to false, count each trace as a separate group of targets hit.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
		bool bCombineTargets;


	void TickTask(float DeltaTime) override;

	FGameplayAbilityTargetDataHandle MakeTargetData(const TArray<TWeakObjectPtr<AActor>>& TargetActors) const;
	
protected:
	float TimeSinceStart;
	UGameplayAbility* OwningAbility;
	FGameplayAbilityTargetDataHandle TargetsCollected;

	FGameplayAbilityTargetingLocationInfo StartLocation;

	void Activate() override;

	virtual void PerformTrace(const FTraceStep& TraceStep);

private:
	float DelayAtStart;
};
