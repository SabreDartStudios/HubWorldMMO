#include "./System/HWAssetManagerStartupJob.h"

TSharedPtr<FStreamableHandle> FHWAssetManagerTaskWithProgress::RunTask() const
{
	TSharedPtr<FStreamableHandle> Handle;
	
	JobFunc(*this, Handle);

	if (Handle.IsValid())
	{
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate::CreateRaw(this, &FHWAssetManagerTaskWithProgress::UpdateSubstepProgressFromStreamable));
		Handle->WaitUntilComplete(0.0f, false);
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate());
	}

	return Handle;
}