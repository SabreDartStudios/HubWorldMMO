// Fill out your copyright notice in the Description page of Project Settings.


#include "./Character/HWCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AHWCharacter::AHWCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHWCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	SetupURO();
}

// Called every frame
void AHWCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHWCharacter::SetupURO()
{
	GetMesh()->AnimUpdateRateParams->bShouldUseLodMap = true;
	GetMesh()->AnimUpdateRateParams->BaseNonRenderedUpdateRate = 4;
	GetMesh()->AnimUpdateRateParams->LODToFrameSkipMap.Add(0, 0);
	GetMesh()->AnimUpdateRateParams->LODToFrameSkipMap.Add(1, 1);
	GetMesh()->AnimUpdateRateParams->LODToFrameSkipMap.Add(2, 2);
	GetMesh()->AnimUpdateRateParams->LODToFrameSkipMap.Add(3, 3);
	GetMesh()->AnimUpdateRateParams->MaxEvalRateForInterpolation = 10;
}

// Called to bind functionality to input
void AHWCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Serialize Base Character Stats to an FString
FString AHWCharacter::SerializeBaseCharacterStats()
{
	FString SerializedBaseCharacterStats = "";
	if (FJsonObjectConverter::UStructToJsonObjectString(BaseCharacterStats, SerializedBaseCharacterStats))
	{
		return SerializedBaseCharacterStats;
	}
	return "";
}

// Deserialize an FString to Base Character Stats
void AHWCharacter::LoadBaseCharacterStatsFromJSON(FString BaseCharacterStatsJSON)
{
	FJsonObjectConverter::JsonObjectStringToUStruct(BaseCharacterStatsJSON, &BaseCharacterStats);
}

// Serialize Base Character Skills to an FString
FString AHWCharacter::SerializeBaseCharacterSkills()
{
	FString SerializedBaseCharacterSkills = "";
	if (FJsonObjectConverter::UStructToJsonObjectString(BaseCharacterSkills, SerializedBaseCharacterSkills))
	{
		return SerializedBaseCharacterSkills;
	}
	return "";
}

// Deserialize an FString to Base Character Skills
void AHWCharacter::LoadBaseCharacterSkillsFromJSON(FString BaseCharacterSkillsJSON)
{
	FJsonObjectConverter::JsonObjectStringToUStruct(BaseCharacterSkillsJSON, &BaseCharacterSkills);
}

// Serialize Character Experience to an FString
FString AHWCharacter::SerializeCharacterExperience()
{
	FString SerializedCharacterExperience = "";
	if (FJsonObjectConverter::UStructToJsonObjectString(CharacterExperience, SerializedCharacterExperience))
	{
		return SerializedCharacterExperience;
	}
	return "";
}

// Deserialize an FString to Character Experience
void AHWCharacter::LoadCharacterExperienceFromJSON(FString CharacterExperienceJSON)
{
	FJsonObjectConverter::JsonObjectStringToUStruct(CharacterExperienceJSON, &CharacterExperience);
}

void AHWCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Only replicate to owner for stats UI
	DOREPLIFETIME_CONDITION(AHWCharacter, BaseCharacterStats, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AHWCharacter, BaseCharacterSkills, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AHWCharacter, CharacterExperience, COND_OwnerOnly);

}