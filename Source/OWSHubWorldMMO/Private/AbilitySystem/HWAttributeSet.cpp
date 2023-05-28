// Copyright 2023 Sabre Dart Studios


#include "AbilitySystem/HWAttributeSet.h"

#include "AbilitySystem/HWAbilitySystemComponent.h"


UWorld* UHWAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UHWAbilitySystemComponent* UHWAttributeSet::GetHWAbilitySystemComponent() const
{
	return Cast<UHWAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}