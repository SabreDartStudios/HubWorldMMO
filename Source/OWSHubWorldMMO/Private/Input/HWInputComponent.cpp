// Copyright 2022 Sabre Dart Studios


#include "./Input/HWInputComponent.h"
#include "Containers/Map.h"
#include "EnhancedInputSubsystems.h"
#include "./Input/HWMappableConfigPair.h"
#include "./Settings/HWSettingsLocal.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputCoreTypes.h"
#include "UObject/NameTypes.h"
#include "UObject/UnrealNames.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HWInputComponent)

class UHWInputConfig;

UHWInputComponent::UHWInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UHWInputComponent::AddInputMappings(const UHWInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void UHWInputComponent::RemoveInputMappings(const UHWInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void UHWInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}