// Copyright 2022 Sabre Dart Studios


#include "HWSettingsLocal.h"
#include "PlayerMappableInputConfig.h"
#include "EnhancedInputSubsystems.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HWSettingsLocal)

UHWSettingsLocal* UHWSettingsLocal::Get()
{
	return GEngine ? CastChecked<UHWSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}

void UHWSettingsLocal::RegisterInputConfig(const UPlayerMappableInputConfig* NewConfig, const bool bIsActive)
{
	if (NewConfig)
	{
		const int32 ExistingConfigIdx = RegisteredInputConfigs.IndexOfByPredicate([&NewConfig](const FLoadedMappableConfigPair& Pair) { return Pair.Config == NewConfig; });
		if (ExistingConfigIdx == INDEX_NONE)
		{
			const int32 NumAdded = RegisteredInputConfigs.Add(FLoadedMappableConfigPair(NewConfig, bIsActive));
			/*if (NumAdded != INDEX_NONE)
			{
				OnInputConfigRegistered.Broadcast(RegisteredInputConfigs[NumAdded]);
			}*/
		}
	}
}

int32 UHWSettingsLocal::UnregisterInputConfig(const UPlayerMappableInputConfig* ConfigToRemove)
{
	if (ConfigToRemove)
	{
		const int32 Index = RegisteredInputConfigs.IndexOfByPredicate([&ConfigToRemove](const FLoadedMappableConfigPair& Pair) { return Pair.Config == ConfigToRemove; });
		if (Index != INDEX_NONE)
		{
			RegisteredInputConfigs.RemoveAt(Index);
			return 1;
		}

	}
	return INDEX_NONE;
}