// Copyright 2023 Sabre Dart Studios


#include "AbilitySystem/HWGameplayAbility_FakeProjectile.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
//#include "./HWAT_WaitMultiTraceForTargets.h"


void UHWGameplayAbility_FakeProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo()))
	{
		//Play Montage And Wait
		/*UAbilityTask_PlayMontageAndWait* Task_PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName("FireProjectileAnim"), DodgingAnimation, DodgingAnimationPlayRate, NAME_None,
			false, 1.f);

		Task_PlayMontage->ReadyForActivation();

		//Wait Delay to Start Spawn Projectile
		UAbilityTask_WaitDelay* Task_SpawnProjectile = UAbilityTask_WaitDelay::WaitDelay(this, DelaySpawnProjectile);
		Task_SpawnProjectile->OnFinish.AddDynamic(this, &UHWGameplayAbility_FakeProjectile::OnDelaySpawnProjectile);

		//Activate the Wait Delay task
		Task_SpawnProjectile->ReadyForActivation();*/
	}
	else
	{
		K2_EndAbility();
	}
}

void UHWGameplayAbility_FakeProjectile::OnDelaySpawnProjectile()
{
	
}