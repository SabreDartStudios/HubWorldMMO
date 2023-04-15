// Fill out your copyright notice in the Description page of Project Settings.


#include "./Character/HWGASPlayerCharacter.h"
#include "../OWSHubWorldMMO.h"
#include "EnhancedInputSubsystems.h"
#include "./Input/HWInputComponent.h"
#include "./AbilitySystem/HWGameplayTags.h"

AHWGASPlayerCharacter::AHWGASPlayerCharacter()
{
	//For players we will use Mixed mode to replicate GE's to the owning client
	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	OnAbilitySystemInitialized.AddUObject(this, &AHWGASPlayerCharacter::AbilitySystemInitialized);
}

UHWAbilitySystemComponent* AHWGASPlayerCharacter::GetHWAbilitySystemComponent()
{
	return Cast<UHWAbilitySystemComponent>(GetAbilitySystemComponent());
}

void AHWGASPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWGASPlayerCharacter - SetupPlayerInputComponent Started"));

	//Setup Input
	if (AHWPlayerController* HWPC = GetController<AHWPlayerController>())
	{
		if (InputComponent != nullptr)
		{
			InitializePlayerInput(InputComponent);
		}
	}
}

void AHWGASPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Grant ability sets
	for (const UHWGameplayAbilitySet* AbilitySet : AbilitySets)
	{
		if (AbilitySet)
		{
			UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWGASPlayerCharacter - PossessedBy AbilitySet Granted"));
			AbilitySet->GiveToAbilitySystem(GetHWAbilitySystemComponent(), nullptr);
		}
	}
}

void AHWGASPlayerCharacter::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWGASPlayerCharacter - InitializePlayerInput Started"));

	const FHWGameplayTags& GameplayTags = FHWGameplayTags::Get();
	
	check(PlayerInputComponent);

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(InputSubsystem);

	InputSubsystem->ClearAllMappings();

	//If there is a valid Input Config
	if (InputConfig)
	{
		// Register any default input configs with the settings so that they will be applied to the player during AddInputMappings
		for (const FMappableConfigPair& Pair : DefaultInputConfigs)
		{
			if (Pair.bShouldActivateAutomatically && Pair.CanBeActivated())
			{
				FModifyContextOptions Options = {};
				Options.bIgnoreAllPressedKeysUntilRelease = false;
				InputSubsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
			}
		}

		UHWInputComponent* HWInputComponent = CastChecked<UHWInputComponent>(PlayerInputComponent);
		HWInputComponent->AddInputMappings(InputConfig, InputSubsystem);

		TArray<uint32> BindHandles;
		HWInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, BindHandles);

		HWInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
		HWInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
		HWInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
		//HWInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
	}

}

void AHWGASPlayerCharacter::AbilitySystemInitialized()
{
	UE_LOG(OWSHubWorldMMO, VeryVerbose, TEXT("AHWGASPlayerCharacter - AbilitySystemInitialized Started"));

	//If there is an Ability Tag Relationship Mapping
	if (AbilitySystem && AbilityTagRelationshipMapping)
	{
		AbilitySystem->SetTagRelationshipMapping(AbilityTagRelationshipMapping);
	}
}

void AHWGASPlayerCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	if (Controller)
	{
		const FVector2D InputValue = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (InputValue.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			AddMovementInput(MovementDirection, InputValue.X);
		}

		if (InputValue.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(MovementDirection, InputValue.Y);
		}
	}
}

void AHWGASPlayerCharacter::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	const FVector2D InputValue = InputActionValue.Get<FVector2D>();

	if (InputValue.X != 0.0f)
	{
		AddControllerYawInput(InputValue.X);
	}

	if (InputValue.Y != 0.0f)
	{
		AddControllerPitchInput(InputValue.Y);
	}
}

void AHWGASPlayerCharacter::Input_LookStick(const FInputActionValue& InputActionValue)
{
	const FVector2D InputValue = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (InputValue.X != 0.0f)
	{
		AddControllerYawInput(InputValue.X * LookGamepadStickYawRate * World->GetDeltaSeconds());
	}

	if (InputValue.Y != 0.0f)
	{
		AddControllerPitchInput(InputValue.Y * LookGamepadStickPitchRate * World->GetDeltaSeconds());
	}
}

void AHWGASPlayerCharacter::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UHWAbilitySystemComponent* ASC = GetHWAbilitySystemComponent())
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void AHWGASPlayerCharacter::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UHWAbilitySystemComponent* ASC = GetHWAbilitySystemComponent())
	{
		ASC->AbilityInputTagReleased(InputTag);
	}
}
