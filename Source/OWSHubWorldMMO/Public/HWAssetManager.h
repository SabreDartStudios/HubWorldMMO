#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "HWAssetManagerStartupJob.h"
#include "HWAssetManager.generated.h"



/**
 * UHWAssetManager
 * You must add this class as your Asset Manager Singleton class in Project Settings.
 */
UCLASS(Config = Game)
class OWSHUBWORLDMMO_API UHWAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	UHWAssetManager();

	static UHWAssetManager& Get();

	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	void AddLoadedAsset(const UObject* Asset);

protected:

	virtual void StartInitialLoading() override;

private:

	void RunStartupTasks();

	TArray<FHWAssetManagerTaskWithProgress> StartupTasks;

	void UpdateGameContentLoadPercent(float GameContentPercent);

	void InitializeAbilitySystem();

	UPROPERTY()
		TSet<TObjectPtr<const UObject>> LoadedAssets;

	FCriticalSection LoadedAssetsCriticalSection;
};


template<typename AssetType>
AssetType* UHWAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template<typename AssetType>
TSubclassOf<AssetType> UHWAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}