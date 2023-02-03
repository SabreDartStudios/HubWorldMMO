// Copyright 2022 Sabre Dart Studios

#pragma once

#include "GameplayTagContainer.h"
#include "UObject/SoftObjectPtr.h"
#include "PlayerMappableInputConfig.h"

#include "HWMappableConfigPair.generated.h"


//class UPlayerMappableInputConfig;

/** A container to organize loaded player mappable configs to their CommonUI input type */
USTRUCT(BlueprintType)
struct FLoadedMappableConfigPair
{
	GENERATED_BODY()

		FLoadedMappableConfigPair() = default;
	FLoadedMappableConfigPair(const UPlayerMappableInputConfig* InConfig, const bool InIsActive)
		: Config(InConfig)
		, bIsActive(InIsActive)
	{}

	/** The player mappable input config that should be applied to the Enhanced Input subsystem */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		TObjectPtr<const UPlayerMappableInputConfig> Config = nullptr;

	/** If this config is currently active. A config is marked as active when it's owning GFA is active */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		bool bIsActive = false;
};

/** A container to organize potentially unloaded player mappable configs to their CommonUI input type */
USTRUCT()
struct FMappableConfigPair
{
	GENERATED_BODY()

		FMappableConfigPair() = default;

	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UPlayerMappableInputConfig> Config;

	/**
	 * Container of platform traits that must be set in order for this input to be activated.
	 *
	 * If the platform does not have one of the traits specified it can still be registered, but cannot
	 * be activated.
	 */
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer DependentPlatformTraits;

	/**
	 * If the current platform has any of these traits, then this config will not be actived.
	 */
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer ExcludedPlatformTraits;

	/** If true, then this input config will be activated when it's associated Game Feature is activated.
	 * This is normally the desirable behavior
	 */
	UPROPERTY(EditAnywhere)
		bool bShouldActivateAutomatically = true;

	/** Returns true if this config pair can be activated based on the current platform traits and settings. */
	bool CanBeActivated() const;

	/**
	 * Registers the given config mapping with the local settings
	 */
	static bool RegisterPair(const FMappableConfigPair& Pair);

	/**
	 * Unregisters the given config mapping with the local settings
	 */
	static void UnregisterPair(const FMappableConfigPair& Pair);
};