// Copyright 2022 Sabre Dart Studios


#include "HWPlayerAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UHWPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MeleeTwistCurveName = "MeleeTwist";

	AimOffsetPitchDampening = 15.f;
	AimOffsetYawDampening = 15.f;

	OwningHWCharacter = Cast<AHWCharacter>(TryGetPawnOwner());
}

void UHWPlayerAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	//BP won't work without this
	Super::NativeUpdateAnimation(DeltaTimeX);

	if (!OwningHWCharacter)
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(OwningHWCharacter->GetCharacterMovement());

	//Is In Air?
	bIsInAir = MovementComponent->IsFalling();

	//Velocity
	Velocity = OwningHWCharacter->GetVelocity();

	//Get Ground Speed
	GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);

	//Is there enough Acceleration to Move?
	if (GroundSpeed > 3.f && MovementComponent->GetCurrentAcceleration() != FVector(0.f, 0.f, 0.f))
	{
		bShouldMove = true;
	}
	else
	{
		bShouldMove = false;
	}

	//Get Speed
	RawSpeedLast = RawSpeed;
	RawSpeed = Velocity.Size();

	//Which foot was last down?
	if (HasMarkerBeenHitThisFrame(FName("WalkRun"), FName("LeftDown")))
	{
		LeftFootLastDown = true;
	}
	else if (HasMarkerBeenHitThisFrame(FName("WalkRun"), FName("RightDown")))
	{
		LeftFootLastDown = false;
	}

	//Get Move Direction
	LeftRightMovement = FVector::DotProduct(OwningHWCharacter->GetActorRightVector(), Velocity);
	ForwardBackwardMovement = FVector::DotProduct(OwningHWCharacter->GetActorForwardVector(), Velocity);

	//Is Montage Playing
	bIsMontagePlaying = IsAnyMontagePlaying();

	//Melee Twist
	MeleeTwist = GetCurveValue(MeleeTwistCurveName);


}