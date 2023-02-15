// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "./Input/HWMappableConfigPair.h"

#include "HWSettingsLocal.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWSettingsLocal : public UGameUserSettings
{
	GENERATED_BODY()
	
public:

	static UHWSettingsLocal* Get();

	/** Get all currently registered input configs */
	const TArray<FLoadedMappableConfigPair>& GetAllRegisteredInputConfigs() const { return RegisteredInputConfigs; }

	/** Register the given input config with the settings to make it available to the player. */
	void RegisterInputConfig(const UPlayerMappableInputConfig* NewConfig, const bool bIsActive);

	/** Unregister the given input config. Returns the number of configs removed. */
	int32 UnregisterInputConfig(const UPlayerMappableInputConfig* ConfigToRemove);

	const TMap<FName, FKey>& GetCustomPlayerInputConfig() const { return CustomKeyboardConfig; }

private:

	/**
	 * Array of currently registered input configs. This is populated by game feature plugins
	 *
	 * @see UGameFeatureAction_AddInputConfig
	 */
	UPROPERTY(VisibleAnywhere)
		TArray<FLoadedMappableConfigPair> RegisteredInputConfigs;

	/** Array of custom key mappings that have been set by the player. Empty by default. */
	UPROPERTY(Config)
		TMap<FName, FKey> CustomKeyboardConfig;
};
