// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "./Character/HWCharacter.h"
#include "HWPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	AHWCharacter* OwningHWCharacter;

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

	//Locomotion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Locomotion)
		FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Locomotion)
		float ForwardBackwardMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Locomotion)
		float LeftRightMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Locomotion)
		float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Locomotion)
		float RawSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Locomotion)
		float RawSpeedLast;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Locomotion)
		bool bAccelerating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Locomotion)
		bool bAcceleratingLast;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Locomotion)
		bool bShouldMove;

	//Jump
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Jump)
		bool bIsInAir;

	//Aim Offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Offset")
		float AimOffsetPitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Offset")
		float AimOffsetYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Offset")
		float SmoothedAimOffsetPitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Offset")
		float SmoothedAimOffsetYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Offset")
		float AimOffsetPitchDampening;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Offset")
		float AimOffsetYawDampening;

	//Last foot down
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stop Walking")
		bool LeftFootLastDown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stop Walking")
		bool StopLeftFootLastDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stop Walking")
		bool LeftPlantLastFrame;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stop Walking")
		bool RightPlantLastFrame;

	//Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bIsMontagePlaying;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float MeleeTwist;

protected:

	FName MeleeTwistCurveName;
	
};
