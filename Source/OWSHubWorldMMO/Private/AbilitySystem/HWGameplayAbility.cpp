// Copyright 2022 Sabre Dart Studios


#include "./AbilitySystem/HWGameplayAbility.h"

UHWGameplayAbility::UHWGameplayAbility()
{
	bServerRespectsRemoteAbilityCancellation = false;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}


AHWGASCharacter* UHWGameplayAbility::GetHWAvatarActor()
{
	AHWGASCharacter* HWGASCharacter = Cast<AHWGASCharacter>(GetAvatarActorFromActorInfo());

	if (!ensure(HWGASCharacter))
	{
		K2_EndAbility();
		return nullptr;
	}

	return HWGASCharacter;
}

void UHWGameplayAbility::EndAbilityThisFrame()
{
	K2_EndAbility();
}

bool UHWGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, 
	OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bAbilityIsBlockedByTags = false;
	bool bRequiredTagsAreMissing = false;

	//Handle the built-in tag blocking system on the ability
	if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
	{
		bAbilityIsBlockedByTags = true;
	}

	//Get the UHWAbilitySystemComponent if the AbilitySystemComponent is of the right type (it should always be)
	const UHWAbilitySystemComponent* AbilitySystem = Cast<UHWAbilitySystemComponent>(&AbilitySystemComponent);

	static FGameplayTagContainer RequiredTagsCollection;
	static FGameplayTagContainer BlockedTagsCollection;

	RequiredTagsCollection = ActivationRequiredTags;
	BlockedTagsCollection = ActivationBlockedTags;

	if (AbilitySystem)
	{
		AbilitySystem->GetAdditionalActivationTagRequirements(AbilityTags, RequiredTagsCollection, BlockedTagsCollection);
	}

	if (BlockedTagsCollection.Num() || RequiredTagsCollection.Num())
	{
		static FGameplayTagContainer AbilitySystemComponentTags;

		AbilitySystemComponentTags.Reset();
		AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);

		if (AbilitySystemComponentTags.HasAny(BlockedTagsCollection))
		{
			/*
			const FLyraGameplayTags& GameplayTags = FLyraGameplayTags::Get();
			if (OptionalRelevantTags && AbilitySystemComponentTags.HasTag(GameplayTags.Status_Death))
			{
				// If player is dead and was rejected due to blocking tags, give that feedback
				OptionalRelevantTags->AddTag(GameplayTags.Ability_ActivateFail_IsDead);
			}
			*/

			bAbilityIsBlockedByTags = true;
		}

		if (!AbilitySystemComponentTags.HasAll(RequiredTagsCollection))
		{
			bRequiredTagsAreMissing = true;
		}
	}

	if (SourceTags != nullptr)
	{
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			if (SourceTags->HasAny(SourceBlockedTags))
			{
				bAbilityIsBlockedByTags = true;
			}

			if (!SourceTags->HasAll(SourceRequiredTags))
			{
				bAbilityIsBlockedByTags = true;
			}
		}
	}

	if (TargetTags != nullptr)
	{
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			if (TargetTags->HasAny(TargetBlockedTags))
			{
				bRequiredTagsAreMissing = true;
			}

			if (!TargetTags->HasAll(TargetRequiredTags))
			{
				bRequiredTagsAreMissing = true;
			}
		}
	}

	if (bAbilityIsBlockedByTags)
	{
		/*
		if (OptionalRelevantTags && BlockedTag.IsValid())
		{
			OptionalRelevantTags->AddTag(BlockedTag);
		}
		*/
		return false;
	}
	if (bRequiredTagsAreMissing)
	{
		return false;
	}

	return true;
}