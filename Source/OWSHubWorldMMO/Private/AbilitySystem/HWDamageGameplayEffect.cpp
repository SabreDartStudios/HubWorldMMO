// Copyright 2022 Sabre Dart Studios


#include "./AbilitySystem/HWDamageGameplayEffect.h"

UHWDamageGameplayEffect::UHWDamageGameplayEffect()
{
	ApplicationTagRequirements.IgnoreTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Combat.State.IFrame")));
}