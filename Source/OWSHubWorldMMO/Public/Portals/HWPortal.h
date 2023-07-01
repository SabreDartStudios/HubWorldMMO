// Copyright 2023 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "HWPortal.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API AHWPortal : public AActor
{
	GENERATED_BODY()
	
public:
	AHWPortal();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
		FString ZoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
		FVector StartingLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
		FRotator StartingRotation;

	UFUNCTION(BlueprintCallable, Category = "Travel")
		void TravelThroughPortal(AActor* ActorFromOverlapEvent);

};
