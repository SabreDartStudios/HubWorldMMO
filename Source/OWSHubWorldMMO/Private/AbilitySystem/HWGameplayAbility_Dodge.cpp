// Copyright 2022 Sabre Dart Studios


#include "./AbilitySystem/HWGameplayAbility_Dodge.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

void UHWGameplayAbility_Dodge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo()))
	{

		AHWGASCharacter* HWGASCharacter = GetHWAvatarActor();
		UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(HWGASCharacter->GetMovementComponent());
		SavedGroundFriction = CharacterMovementComponent->GroundFriction;
		CharacterMovementComponent->GroundFriction = 0.f;

		//Root Motion Contstant Force
		FVector WorldDirection;
		FVector SetVelocityOnFinish;

		//0 = Forwards, 1 = Right, 2 = Backwards, 3 = Left
		if (Direction == 0)
			WorldDirection = HWGASCharacter->GetActorForwardVector();
		else if (Direction == 1)
			WorldDirection = HWGASCharacter->GetActorRightVector();
		else if (Direction == 2)
			WorldDirection = HWGASCharacter->GetActorForwardVector().RotateAngleAxis(180.f, FVector(0.f,0.f,1.f));
		else if (Direction == 3)
			WorldDirection = HWGASCharacter->GetActorRightVector().RotateAngleAxis(180.f, FVector(0.f, 0.f, 1.f));

		UAbilityTask_ApplyRootMotionConstantForce* Task_Dash = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, FName("DashTask"), 
			WorldDirection, Strength, Duration, true, NULL, ERootMotionFinishVelocityMode::ClampVelocity, SetVelocityOnFinish, 250.f, false);
		Task_Dash->OnFinish.AddDynamic(this, &UHWGameplayAbility_Dodge::OnFinished);
		Task_Dash->ReadyForActivation();

		//Play Montage And Wait
		UAbilityTask_PlayMontageAndWait* Task_PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName("DodgeAnim"), DodgingAnimation, DodgingAnimationPlayRate, NAME_None,
			false, 1.f);
		/*
		Task_PlayMontage->OnCancelled.AddDynamic(this, &UHWGameplayAbility_Dodge::OnAnimCancelled);
		Task_PlayMontage->OnInterrupted.AddDynamic(this, &UHWGameplayAbility_Dodge::OnAnimInterrupted);
		Task_PlayMontage->OnAnimNotify.AddDynamic(this, &UHWGameplayAbility_Dodge::OnStartLoopingSection);
		*/
		Task_PlayMontage->ReadyForActivation();

		//Only run the Wait Delay Task if there is a valid DodgedRecentlyGameplayEffect
		if (DodgedRecentlyGameplayEffect)
		{
			//Wait Delay
			//Wait DelayDodgedRecentlyApplication and then call OnDelayDodgedRecently to apply the DodgedRecentlyGameplayEffect
			UAbilityTask_WaitDelay* Task_DelayDodgedRecently = UAbilityTask_WaitDelay::WaitDelay(this, DelayDodgedRecentlyApplication);
			Task_DelayDodgedRecently->OnFinish.AddDynamic(this, &UHWGameplayAbility_Dodge::OnDelayDodgedRecently);

			//Activate the Wait Delay task
			Task_DelayDodgedRecently->ReadyForActivation();
		}

		//Only run the Wait Delay Task if there is a valid DodgedRecentlyGameplayEffect
		if (IFrameGameplayEffect)
		{
			//Wait Delay
			//Wait DelayIFrameApplication and then call OnDelayIFrame to apply the IFrameGameplayEffect
			UAbilityTask_WaitDelay* Task_DelayIFrameApplication = UAbilityTask_WaitDelay::WaitDelay(this, DelayIFrameApplication);
			Task_DelayIFrameApplication->OnFinish.AddDynamic(this, &UHWGameplayAbility_Dodge::OnDelayIFrame);

			//Activate the Wait Delay task
			Task_DelayIFrameApplication->ReadyForActivation();
		}
	}
	else
	{
		K2_EndAbility();
	}
}

void UHWGameplayAbility_Dodge::OnFinished()
{
	AHWGASCharacter* HWGASCharacter = GetHWAvatarActor();
	UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(HWGASCharacter->GetMovementComponent());
	CharacterMovementComponent->GroundFriction = SavedGroundFriction;

	K2_EndAbility();
}

void UHWGameplayAbility_Dodge::OnDelayDodgedRecently()
{
	BP_ApplyGameplayEffectToOwner(DodgedRecentlyGameplayEffect, 1, 1);
}

void UHWGameplayAbility_Dodge::OnDelayIFrame()
{
	BP_ApplyGameplayEffectToOwner(IFrameGameplayEffect, 1, 1);
}