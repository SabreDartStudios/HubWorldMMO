// Copyright 2022 Sabre Dart Studios


#include "./AbilitySystem/HWAT_WaitMultiTraceForTargets.h"
#include "Kismet/KismetMathLibrary.h"

UHWAT_WaitMultiTraceForTargets::UHWAT_WaitMultiTraceForTargets(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	DelayAtStart = 99.f; //99 Seconds is the highest value that can be used for StartTimeInSeconds in the Trace Steps
	bCombineTargets = true;
}

UHWAT_WaitMultiTraceForTargets* UHWAT_WaitMultiTraceForTargets::WaitMultiTraceForTargets(UGameplayAbility* OwningAbility, FName TaskInstanceName, TArray<FTraceStep> TraceSteps, bool CombineTargets)
{
	UHWAT_WaitMultiTraceForTargets* MyObj = NewAbilityTask<UHWAT_WaitMultiTraceForTargets>(OwningAbility, TaskInstanceName);		//Register for task list here, providing a given FName as a key
	MyObj->OwningAbility = OwningAbility;
	MyObj->TraceSteps = TraceSteps;
	MyObj->bCombineTargets = CombineTargets;
	return MyObj;
}

void UHWAT_WaitMultiTraceForTargets::Activate()
{
	TimeSinceStart = 0.f;
	int32 CountOfTraceSteps = 0;
	DelayAtStart = 99.f;
	for (auto TraceStep : TraceSteps)
	{
		CountOfTraceSteps++;
		if (TraceStep.StartTimeInSeconds < DelayAtStart)
		{
			DelayAtStart = TraceStep.StartTimeInSeconds;
		}
	}

	if (CountOfTraceSteps < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("UHWAT_WaitMultiTraceForTargets - One or more trace steps are required!"));
		EndTask();
	}
}

void UHWAT_WaitMultiTraceForTargets::TickTask(float DeltaTime)
{
	//Accumulate time to keep track of how long it has been since this task was activated
	TimeSinceStart += DeltaTime;

	if (TimeSinceStart >= DelayAtStart)
	{
		bool bAllStepsHaveBeenActivated = true;
		//Loop through all the Trace Steps
		for (auto& TraceStep : TraceSteps)
		{
			//Skip Activated steps or disabled steps
			if (!TraceStep.bActivated && !TraceStep.bDisabled)
			{
				//We found at least one step that hasn't been activated yet, so don't end this task
				bAllStepsHaveBeenActivated = false;

				if (TimeSinceStart >= TraceStep.StartTimeInSeconds)
				{
					//Mark this step as already activated so we don't run it twice
					TraceStep.bActivated = true;
					//Perform the trace for this step
					PerformTrace(TraceStep);
				}
			}
		}
		
		//If we are combining the targets and all of the trace steps have been activated.  Let's return our targets to the Valid Data callback.
		if (bAllStepsHaveBeenActivated)
		{
			//If we are combining the targets, let's return our targets to the Valid Data callback.
			if (bCombineTargets)
			{
				OnTargetDataReadyCallback(TargetsCollected);
			}
			else //If we aren't combining targets, we call Finished
			{
				Finished.Broadcast();
			}
		}
	}
}

//Perform trace for a single step
void UHWAT_WaitMultiTraceForTargets::PerformTrace(const FTraceStep& TraceStep)
{
	//Get FGameplayAbilityActorInfo from OwningAbility
	const FGameplayAbilityActorInfo* Info = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);

	//Calculate world offset
	//FVector TraceOrigin = Info->AvatarActor->GetActorLocation() + TraceStep.OffsetFromOrigin;

	FTransform TraceOrigin = UKismetMathLibrary::ComposeTransforms(FTransform(FRotator::ZeroRotator, TraceStep.OffsetFromOrigin, FVector::OneVector), Info->AvatarActor->GetTransform());

	//Declare an array to collect overlaps
	TArray<FOverlapResult> Overlaps;

	//Sphere collision trace
	if (TraceStep.CollisionShape == 0)
	{
		bool bTraceComplex = false;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(RadiusTargetingOverlap), bTraceComplex);
		Params.bReturnPhysicalMaterial = false;
		Info->AvatarActor->GetWorld()->OverlapMultiByObjectType(Overlaps, TraceOrigin.GetTranslation(), FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(TraceStep.Radius), Params);

		if (TraceStep.bDebug)
		{
			DrawDebugSphere(GetWorld(), TraceOrigin.GetTranslation(), TraceStep.Radius, 12, FColor::Red, false, 10, 1, 2.f);
		}
	}

	TArray<TWeakObjectPtr<AActor>>	HitActors;
	for (int32 i = 0; i < Overlaps.Num(); ++i)
	{
		//Should this check to see if these pawns are in the AimTarget list?
		APawn* PawnActor = Cast<APawn>(Overlaps[i].GetActor());
		//Make sure the reference is valid, we aren't inserting a duplicate target, and we aren't hitting ourselves if we don't want to.
		if (PawnActor && !HitActors.Contains(PawnActor) && (PawnActor != Info->AvatarActor || TraceStep.bCanHitSelf))
		{
			HitActors.Add(PawnActor);
			if (TraceStep.bDebug)
			{
				DrawDebugLine(GetWorld(), TraceOrigin.GetTranslation(), PawnActor->GetActorLocation(), FColor::Yellow, false, 10, 2, 2);
				DrawDebugPoint(GetWorld(), PawnActor->GetActorLocation(), 30, FColor::Green, false, 10, 3);
				UE_LOG(LogTemp, Error, TEXT("Drawing Hit!"));
			}
		}
	}

	TargetsCollected = MakeTargetData(HitActors);
}

void UHWAT_WaitMultiTraceForTargets::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data)
{
	check(AbilitySystemComponent.Get());
	if (!Ability)
	{
		return;
	}

	const FGameplayAbilityTargetData* AbilityTargetData = Data.Get(0);
	TArray<TWeakObjectPtr<AActor>> ActorsHit = AbilityTargetData->GetActors();

	ValidData.Broadcast(Data);

	//If we are combining the targets, then end the task after we return them.  If we are aren't combining the targets then there may be additional calls to the ValidData delegate
	if (bCombineTargets)
	{
		//Call Finished and then end the tasks
		Finished.Broadcast();
		EndTask();
	}
}

FGameplayAbilityTargetDataHandle UHWAT_WaitMultiTraceForTargets::MakeTargetData(const TArray<TWeakObjectPtr<AActor>>& TargetActors) const
{
	/** Note: This will be cleaned up by the FGameplayAbilityTargetDataHandle (via an internal TSharedPtr) */
	return StartLocation.MakeTargetDataHandleFromActors(TargetActors);
}