#include "AbilityTask_WaitLanded.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"


UAbilityTask_WaitLanded::UAbilityTask_WaitLanded(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

UAbilityTask_WaitLanded* UAbilityTask_WaitLanded::WaitLanded(UGameplayAbility* OwningAbility)
{
	UAbilityTask_WaitLanded* MyObj = NewAbilityTask<UAbilityTask_WaitLanded>(OwningAbility);
	return MyObj;
}

void UAbilityTask_WaitLanded::Activate()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
	if (Character)
	{
		Character->LandedDelegate.AddDynamic(this, &UAbilityTask_WaitLanded::OnLandedCallback);
	}
	SetWaitingOnAvatar();
}

void UAbilityTask_WaitLanded::OnLandedCallback(const FHitResult& Hit)
{
	bHasLanded = true;

	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
	if (Character && Character->bClientUpdating)
	{
		// If in a move replay, we just mark that we landed so that next tick we trigger landed
	}
	else
	{
		TriggerLanded();
	}
}

void UAbilityTask_WaitLanded::TriggerLanded()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnLanded.Broadcast();
	}
}

void UAbilityTask_WaitLanded::OnDestroy(bool AbilityEnded)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
	if (Character)
	{
		Character->LandedDelegate.RemoveDynamic(this, &UAbilityTask_WaitLanded::OnLandedCallback);
	}

	Super::OnDestroy(AbilityEnded);
}