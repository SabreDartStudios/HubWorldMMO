// Copyright 2023 Sabre Dart Studios


#include "AbilitySystem/HWFakeProjectile.h"
#include "Engine/EngineTypes.h"
#include "../OWSHubWorldMMO.h"


// Sets default values
AHWFakeProjectile::AHWFakeProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicatingMovement(false);
	bNetUseOwnerRelevancy = true;
	bOnlyRelevantToOwner = false;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWFakeProjectile - Constructor"));
}

// Called when the game starts or when spawned
void AHWFakeProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHWFakeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AHWFakeProjectile::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	return !IsOwnedBy(ViewTarget);
}