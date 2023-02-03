#include "HWAssetManager.h"
#include "HWGameplayTags.h"
#include "HWLogging.h"
#include "AbilitySystemGlobals.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HWAssetManager)

#define STARTUP_TASK_WITH_WEIGHT(TaskFunc, TaskWeight) StartupTasks.Add(FHWAssetManagerTaskWithProgress(#TaskFunc, [this](const FHWAssetManagerTaskWithProgress& StartupTask, TSharedPtr<FStreamableHandle>& LoadHandle){TaskFunc;}, TaskWeight))
#define STARTUP_TASK(TaskFunc) STARTUP_TASK_WITH_WEIGHT(TaskFunc, 1.f)

UHWAssetManager::UHWAssetManager()
{
	
}

void UHWAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("UHWAssetManager::StartInitialLoading");

	Super::StartInitialLoading();

	STARTUP_TASK(InitializeAbilitySystem());

	RunStartupTasks();
}

UHWAssetManager& UHWAssetManager::Get()
{
	check(GEngine);

	if (UHWAssetManager* Singleton = Cast<UHWAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	return *NewObject<UHWAssetManager>();
}

UObject* UHWAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		return AssetPath.TryLoad();
	}

	return nullptr;
}

void UHWAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCriticalSection);
		LoadedAssets.Add(Asset);
	}
}

void UHWAssetManager::InitializeAbilitySystem()
{
	SCOPED_BOOT_TIMING("UHWAssetManager::InitializeAbilitySystem");

	FHWGameplayTags::InitializeNativeTags();

	UAbilitySystemGlobals::Get().InitGlobalData();
}

void UHWAssetManager::RunStartupTasks()
{
	SCOPED_BOOT_TIMING("UHWAssetManager::RunStartupTasks");

	if (IsRunningDedicatedServer())
	{
		for (const FHWAssetManagerTaskWithProgress& StartupTask : StartupTasks)
		{
			StartupTask.RunTask();
		}

		StartupTasks.Empty();
		return;
	}

	if (StartupTasks.IsEmpty())
	{
		UpdateGameContentLoadPercent(1.0f);
	}
	else //There are Startup Tasks to process
	{
		float SumOfAllTaskWeights = 0.0f;
		for (const FHWAssetManagerTaskWithProgress& StartupTask : StartupTasks)
		{
			SumOfAllTaskWeights += StartupTask.JobWeight;
		}

		float AccumulatedProgress = 0.0f;
		for (FHWAssetManagerTaskWithProgress& StartupTask : StartupTasks)
		{
			const float StartupTaskJobWeight = StartupTask.JobWeight;
			StartupTask.SubstepProgressDelegate.BindLambda([This = this, AccumulatedProgress, StartupTaskJobWeight, SumOfAllTaskWeights](float NewProgress)
				{
					const float SubstepAdjustment = FMath::Clamp(NewProgress, 0.0f, 1.0f) * StartupTaskJobWeight;
					const float OverallPercentWithSubstep = (AccumulatedProgress + SubstepAdjustment) / SumOfAllTaskWeights;

					This->UpdateGameContentLoadPercent(OverallPercentWithSubstep);
				});

			StartupTask.RunTask();

			StartupTask.SubstepProgressDelegate.Unbind();

			AccumulatedProgress += StartupTaskJobWeight;

			UpdateGameContentLoadPercent(AccumulatedProgress / SumOfAllTaskWeights);
		}

		StartupTasks.Empty();
	}
}

void UHWAssetManager::UpdateGameContentLoadPercent(float GameContentPercent)
{
	// Could route this to the early startup loading screen
	UE_LOG(LogHubWorldMMO, Log, TEXT("Loading... %.0f"), GameContentPercent);
}