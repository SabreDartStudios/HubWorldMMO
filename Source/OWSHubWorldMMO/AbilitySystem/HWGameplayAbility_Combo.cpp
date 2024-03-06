// Copyright 2023 Sabre Dart Studios


#include "HWGameplayAbility_Combo.h"

void UHWGameplayAbility_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//LockAvatarInput(true);
	bAdvanceToNextCombo = false;

	if (CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo()))
	{
		UE_LOG(LogTemp, VeryVerbose, TEXT("[UHWGameplayAbility_Combo::ActivateAbility] CommitAbility Success: %s - %d"), (GetCurrentActivationInfo().ActivationMode == EGameplayAbilityActivationMode::Authority ? TEXT("Server") : TEXT("Client")), GetUniqueID());
		//AActor* AvatarActor = GetAvatarActorFromActorInfo();

		//Call OnInputRelease when the input key used to activate this ability has been released
		AbilityTaskInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
		if (IsValid(AbilityTaskInputRelease))
		{
			AbilityTaskInputRelease->OnRelease.AddDynamic(this, &ThisClass::OnInputRelease);
			AbilityTaskInputRelease->Activate();
		}

		//Call OnBranchingPointReached [BranchingPointTime] seconds from now
		AbilityTaskWaitDelayBranchingPoint = UAbilityTask_WaitDelay::WaitDelay(this, BranchingPointTime);
		if (IsValid(AbilityTaskWaitDelayBranchingPoint))
		{
			AbilityTaskWaitDelayBranchingPoint->OnFinish.AddDynamic(this, &ThisClass::UHWGameplayAbility_Combo::OnBranchingPointReached);
			AbilityTaskWaitDelayBranchingPoint->Activate();
		}

		//Call OnDamageTimeStampReached [DamageTimeStamp] seconds from now
		AbilityTaskWaitDelayDamageTimeStamp = UAbilityTask_WaitDelay::WaitDelay(this, DamageTimeStamp);
		if (IsValid(AbilityTaskWaitDelayDamageTimeStamp))
		{
			AbilityTaskWaitDelayDamageTimeStamp->OnFinish.AddDynamic(this, &ThisClass::UHWGameplayAbility_Combo::OnDamageTimeStampReached);
			AbilityTaskWaitDelayDamageTimeStamp->Activate();
		}

		AbilityStart();
	}
	else
	{
		K2_EndAbility();
	}
}

void UHWGameplayAbility_Combo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, VeryVerbose, TEXT("[UHWGameplayAbility_Combo::EndAbility] Started: %s - %d"), (GetCurrentActivationInfo().ActivationMode == EGameplayAbilityActivationMode::Authority ? TEXT("Server") : TEXT("Client")), GetUniqueID());

	//LockAvatarInput(false);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHWGameplayAbility_Combo::OnInputRelease(float TimeHeld)
{
	UE_LOG(LogTemp, VeryVerbose, TEXT("[UHWGameplayAbility_Combo::OnInputRelease] Started: %s - %d"), (GetCurrentActivationInfo().ActivationMode == EGameplayAbilityActivationMode::Authority ? TEXT("Server") : TEXT("Client")), GetUniqueID());

	//Stop listening for input release
	if (IsValid(AbilityTaskInputRelease))
	{
		AbilityTaskInputRelease->EndTask();
	}

	AbilityTaskInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this, true);
	if (IsValid(AbilityTaskInputPress))
	{
		AbilityTaskInputPress->OnPress.AddDynamic(this, &ThisClass::OnInputPress);
		AbilityTaskInputPress->Activate();
	}
}

void UHWGameplayAbility_Combo::OnInputPress(float TimeWaited)
{
	UE_LOG(LogTemp, VeryVerbose, TEXT("[UHWGameplayAbility_Combo::OnInputPress] Started: %s - %d"), (GetCurrentActivationInfo().ActivationMode == EGameplayAbilityActivationMode::Authority ? TEXT("Server") : TEXT("Client")), GetUniqueID());

	//Stop listening for input press
	if (IsValid(AbilityTaskInputPress))
	{
		AbilityTaskInputPress->EndTask();
	}

	bAdvanceToNextCombo = true;
}

void UHWGameplayAbility_Combo::OnBranchingPointReached()
{
	UE_LOG(LogTemp, VeryVerbose, TEXT("[UHWGameplayAbility_Combo::OnBranchingPointReached] Started: %s - %d"), (GetCurrentActivationInfo().ActivationMode == EGameplayAbilityActivationMode::Authority ? TEXT("Server") : TEXT("Client")), GetUniqueID());

	if (bAdvanceToNextCombo)
	{
		if (NextComboTag != FGameplayTag::EmptyTag)
		{
			UE_LOG(LogTemp, VeryVerbose, TEXT("[UHWGameplayAbility_Combo::OnBranchingPointReached] Sent Next Combo Gameplay Event: %s - %d"), (GetCurrentActivationInfo().ActivationMode == EGameplayAbilityActivationMode::Authority ? TEXT("Server") : TEXT("Client")), GetUniqueID());
			FGameplayEventData GameplayEventData;
			SendGameplayEvent(NextComboTag, GameplayEventData);
		}

		K2_EndAbility();
	}
	else
	{
		if (AnimationDuration - BranchingPointTime > 0.f)
		{
			//Call OnAnimationEnded [AnimationDuration] - [BranchingPointTime] seconds from now
			AbilityTaskWaitDelayAnimationDuration = UAbilityTask_WaitDelay::WaitDelay(this, AnimationDuration - BranchingPointTime);
			if (IsValid(AbilityTaskWaitDelayAnimationDuration))
			{
				AbilityTaskWaitDelayAnimationDuration->OnFinish.AddDynamic(this, &ThisClass::UHWGameplayAbility_Combo::OnAnimationEnded);
				AbilityTaskWaitDelayAnimationDuration->Activate();
			}
		}
		else
		{
			K2_EndAbility();
		}
	}
}

void UHWGameplayAbility_Combo::OnDamageTimeStampReached()
{
	UE_LOG(LogTemp, VeryVerbose, TEXT("[UHWGameplayAbility_Combo::OnDamageTimeStampReached] Started: %s - %d"), (GetCurrentActivationInfo().ActivationMode == EGameplayAbilityActivationMode::Authority ? TEXT("Server") : TEXT("Client")), GetUniqueID());

	AbilityApplyDamage();
}

void UHWGameplayAbility_Combo::OnAnimationEnded()
{
	UE_LOG(LogTemp, VeryVerbose, TEXT("[UHWGameplayAbility_Combo::OnAnimationEnded] Started: %s - %d"), (GetCurrentActivationInfo().ActivationMode == EGameplayAbilityActivationMode::Authority ? TEXT("Server") : TEXT("Client")), GetUniqueID());

	K2_EndAbility();
}