// Copyright 2022 Sabre Dart Studios


#include "./AbilitySystem/HWATA_ConeTrace.h"
#include "DrawDebugHelpers.h"

void AHWATA_ConeTrace::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (SourceActor)
	{
		FVector Origin = StartLocation.GetTargetingTransform().GetLocation();
		FGameplayAbilityTargetDataHandle Handle = MakeTargetData(PerformOverlap(Origin), Origin);
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}

TArray<TWeakObjectPtr<AActor>> AHWATA_ConeTrace::PerformOverlap(const FVector& Origin)
{
	bool bTraceComplex = false;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(RadiusTargetingOverlap), bTraceComplex);
	Params.bReturnPhysicalMaterial = false;

	TArray<FOverlapResult> Overlaps;

	if (bDebug)
	{
		DrawDebugAltCone(GetWorld(), Origin, ForwardVector.Rotation(), Radius, FMath::DegreesToRadians(HalfAngle * 2.f), FMath::DegreesToRadians(HalfAngle * 2.f), FColor::Red, true, 1000, 1, 2.f);
	}
	FCollisionShape ConeToTrace = FCollisionShape::MakeSphere(Radius);
	SourceActor->GetWorld()->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn), ConeToTrace, Params);

	TArray<TWeakObjectPtr<AActor>>	HitActors;

	for (int32 i = 0; i < Overlaps.Num(); ++i)
	{
		//Should this check to see if these pawns are in the AimTarget list?
		APawn* PawnActor = Cast<APawn>(Overlaps[i].GetActor());
		if (PawnActor && !HitActors.Contains(PawnActor) && Filter.FilterPassesForActor(PawnActor))
		{
			FVector ActorOrigin;
			FVector ActorBoxExtent;
			PawnActor->GetActorBounds(true, ActorOrigin, ActorBoxExtent);

			FVector Midpoint = (ActorOrigin + (ActorOrigin + ActorBoxExtent)) / 2;

			FVector ToMidPoint = Midpoint - Origin;
			ToMidPoint = ToMidPoint.GetSafeNormal();
			float AngleToCompare = FMath::Acos(FVector::DotProduct(ToMidPoint, ForwardVector));

			//UE_LOG(LogTemp, Error, TEXT("Angle to Compare: %f"), AngleToCompare);

			if (AngleToCompare <= FMath::DegreesToRadians(HalfAngle))
			{
				//UE_LOG(LogTemp, Error, TEXT("We are in the cone!"));

				HitActors.Add(PawnActor);
			}
		}
	}

	return HitActors;
}