#include "./AbilitySystem/HWAbilitySystemComponent.h"
#include "./AbilitySystem/HWGameplayAbility.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_AbilityInputBlocked, "Gameplay.AbilityInputBlocked");

UHWAbilitySystemComponent::UHWAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//This function uses InvokeReplicatedEvent to replicate Input Pressed events up to the server to support the Input related Ability Tasks.
void UHWAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

//This function uses InvokeReplicatedEvent to replicate Input Released events up to the server to support the Input related Ability Tasks.
void UHWAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

//This function is called to collect Ability Input Pressed events for processing later in the ProcessAbilityInput function
void UHWAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		//Find an ability in the ActivatableAbilities ray for the InputTag
		FGameplayAbilitySpec* FoundAbilitySpec = ActivatableAbilities.Items.FindByPredicate(
			[&InputTag](const FGameplayAbilitySpec& AbilitySpec) { return AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag); });

		//If we find it and it has a valid Ability, then add it to the InputPressedSpecHandles and InputHeldSpecHandles arrays
		if (FoundAbilitySpec && FoundAbilitySpec->Ability)
		{
			InputPressedSpecHandles.AddUnique(FoundAbilitySpec->Handle);
			InputHeldSpecHandles.AddUnique(FoundAbilitySpec->Handle);
		}
	}
}

//This function is called to collect Ability Input Released events for processing later in the ProcessAbilityInput function
void UHWAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		//Find an ability in the ActivatableAbilities ray for the InputTag
		FGameplayAbilitySpec* FoundAbilitySpec = ActivatableAbilities.Items.FindByPredicate(
			[&InputTag](const FGameplayAbilitySpec& AbilitySpec) { return AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag); });

		//If we find it and it has a valid Ability, then add it to the InputReleasedSpecHandles and InputHeldSpecHandles arrays
		if (FoundAbilitySpec && FoundAbilitySpec->Ability)
		{
			InputReleasedSpecHandles.AddUnique(FoundAbilitySpec->Handle);
			InputHeldSpecHandles.Remove(FoundAbilitySpec->Handle);
		}
	}
}

void UHWAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	//If there is a Gameplay Ability tag TAG_Gameplay_AbilityInputBlocked that is blocking input, then don't process input events this frame
	if (HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
	{
		//Clear the input arrays so the events don't get processed later when the blocking tag is removed.
		ClearAbilityInput();
		return;
	}

	static TArray<FGameplayAbilitySpecHandleWithBool> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	//Handle InputHeldSpecHandles this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UHWGameplayAbility* HWAbilityCDO = CastChecked<UHWGameplayAbility>(AbilitySpec->Ability);

				if (HWAbilityCDO->GetActivationPolicy() == EHWAbilityActivationPolicy::WhileInputActive)
				{
					FGameplayAbilitySpecHandleWithBool GameplayAbilitySpecHandleWithBoolToAdd;
					GameplayAbilitySpecHandleWithBoolToAdd.GameplayAbilitySpecHandle = AbilitySpec->Handle;
					GameplayAbilitySpecHandleWithBoolToAdd.bEndAbilityThisFrame = HWAbilityCDO->GetEndAbilityThisFrame();
					AbilitiesToActivate.AddUnique(GameplayAbilitySpecHandleWithBoolToAdd);
				}
			}
		}
	}

	//Handle InputPressedSpecHandles this frame
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				const UHWGameplayAbility* HWAbilityCDO = CastChecked<UHWGameplayAbility>(AbilitySpec->Ability);

				//If the ability is already active and the ability has bShouldReplicateInputPressedEventsWhenActive set to true, replicate the input pressed event to support Input related Ability Tasks
				if (AbilitySpec->IsActive() && HWAbilityCDO->GetShouldReplicateInputPressedEventsWhenActive())
				{
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					if (!AbilitySpec->InputPressed && HWAbilityCDO->GetActivationPolicy() == EHWAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitySpec->InputPressed = true;

						FGameplayAbilitySpecHandleWithBool GameplayAbilitySpecHandleWithBoolToAdd;
						GameplayAbilitySpecHandleWithBoolToAdd.GameplayAbilitySpecHandle = AbilitySpec->Handle;
						GameplayAbilitySpecHandleWithBoolToAdd.bEndAbilityThisFrame = HWAbilityCDO->GetEndAbilityThisFrame();
						AbilitiesToActivate.AddUnique(GameplayAbilitySpecHandleWithBoolToAdd);
					}
				}
			}
		}
	}

	//Try to activate abilities in the InputPressedSpecHandles and InputReleasedSpecHandles arrays
	for (const FGameplayAbilitySpecHandleWithBool& GameplayAbilitySpecHandleWithBoolToAdd : AbilitiesToActivate)
	{
		if (GameplayAbilitySpecHandleWithBoolToAdd.bEndAbilityThisFrame)
		{
			TryActivateAbilityBatchedAndEndInSameFrame(GameplayAbilitySpecHandleWithBoolToAdd.GameplayAbilitySpecHandle);
		}
		else 
		{
			TryActivateAbility(GameplayAbilitySpecHandleWithBoolToAdd.GameplayAbilitySpecHandle);
		}
	}

	//Inputs released this frame are replicated to the server with InvokeReplicatedEvent in AbilitySpecInputReleased() to support Input related Ability Tasks
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				const UHWGameplayAbility* HWAbilityCDO = CastChecked<UHWGameplayAbility>(AbilitySpec->Ability);

				//If the ability is already active and the ability has bShouldReplicateInputReleasedEventsWhenActive set to true, replicate the input released event to support Input related Ability Tasks
				if (AbilitySpec->IsActive() && HWAbilityCDO->GetShouldReplicateInputReleasedEventsWhenActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	//Empty the InputPressedSpecHandles and InputReleasedSpecHandles arrays so they don't get triggered again.  Don't empty InputHeldSpecHandles as it needs to cary over to the next frame.
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UHWAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

bool UHWAbilitySystemComponent::TryActivateAbilityBatchedAndEndInSameFrame(FGameplayAbilitySpecHandle AbilityHandle)
{
	bool DidAbilityActivateSuccessfully = false;
	if (AbilityHandle.IsValid())
	{
		FScopedServerAbilityRPCBatcher ScopedServerAbilityRPCBatcher(this, AbilityHandle);
		DidAbilityActivateSuccessfully = TryActivateAbility(AbilityHandle, true);

		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(AbilityHandle);
		if (AbilitySpec)
		{
			UHWGameplayAbility* HWGameplayAbility = Cast<UHWGameplayAbility>(AbilitySpec->GetPrimaryInstance());
			HWGameplayAbility->EndAbilityThisFrame();
		}

		return DidAbilityActivateSuccessfully;
	}

	return DidAbilityActivateSuccessfully;
}

void UHWAbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

void UHWAbilitySystemComponent::SetTagRelationshipMapping(UHWAbilityTagRelationshipMapping* NewMapping)
{
	TagRelationshipMapping = NewMapping;
}