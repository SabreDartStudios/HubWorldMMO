#pragma once

#include "Containers/UnrealString.h"
#include "Delegates/Delegate.h"
#include "Engine/StreamableManager.h"
#include "HAL/PlatformTime.h"
#include "Templates/Function.h"
#include "Templates/SharedPointer.h"

DECLARE_DELEGATE_OneParam(FHWAssetManagerStartupJobSubstepProgress, float);

struct FHWAssetManagerTaskWithProgress
{
	FHWAssetManagerStartupJobSubstepProgress SubstepProgressDelegate;
	TFunction<void(const FHWAssetManagerTaskWithProgress&, TSharedPtr<FStreamableHandle>&)> JobFunc;
	FString JobName;
	float JobWeight;
	mutable double LastUpdate = 0;

	FHWAssetManagerTaskWithProgress(const FString& InJobName, const TFunction<void(const FHWAssetManagerTaskWithProgress&, TSharedPtr<FStreamableHandle>&)>& InJobFunc, float InJobWeight)
		: JobFunc(InJobFunc)
		, JobName(InJobName)
		, JobWeight(InJobWeight)
	{}

	TSharedPtr<FStreamableHandle> RunTask() const;

	void UpdateSubstepProgress(float NewProgress) const
	{
		SubstepProgressDelegate.ExecuteIfBound(NewProgress);
	}

	void UpdateSubstepProgressFromStreamable(TSharedRef<FStreamableHandle> StreamableHandle) const
	{
		if (SubstepProgressDelegate.IsBound())
		{
			// StreamableHandle::GetProgress traverses() a large graph and is quite expensive
			double Now = FPlatformTime::Seconds();
			if (LastUpdate - Now > 1.0 / 60)
			{
				SubstepProgressDelegate.Execute(StreamableHandle->GetProgress());
				LastUpdate = Now;
			}
		}
	}
};
