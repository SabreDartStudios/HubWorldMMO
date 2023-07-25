// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HWCharacter.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FHWBaseCharacterStats
{
	GENERATED_USTRUCT_BODY()

	FHWBaseCharacterStats() 
	{
		Strength = 10;
		Agility = 10;
		Constitution = 10;
		CharacterLevel = 1;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		int32 Strength;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		int32 Agility;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		int32 Constitution;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		int32 CharacterLevel;

};


USTRUCT(BlueprintType, Blueprintable)
struct FHWBaseCharacterSkills
{
	GENERATED_USTRUCT_BODY()

	FHWBaseCharacterSkills()
	{
		Skill1 = 0;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skills")
		int32 Skill1;

};


USTRUCT(BlueprintType, Blueprintable)
struct FHWCharacterExperience
{
	GENERATED_USTRUCT_BODY()

	FHWCharacterExperience()
	{
		XP = 10;
		NumberOfKills = 10;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		int32 XP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
		int32 NumberOfKills;

};


UCLASS()
class OWSHUBWORLDMMO_API AHWCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHWCharacter();

	// Base Character Stats Struct (Replicated)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Stats")
		FHWBaseCharacterStats BaseCharacterStats;

	// Serialize Base Character Stats to an FString
	UFUNCTION(BlueprintCallable, Category = "Stats")
		FString SerializeBaseCharacterStats();

	// Deserialize an FString to Base Character Stats
	UFUNCTION(BlueprintCallable, Category = "Stats")
		void LoadBaseCharacterStatsFromJSON(FString BaseCharacterStatsJSON);

	// Base Character Skills Struct
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Skills")
		FHWBaseCharacterSkills BaseCharacterSkills;

	// Serialize Base Character Skills to an FString
	UFUNCTION(BlueprintCallable, Category = "Stats")
		FString SerializeBaseCharacterSkills();

	// Deserialize an FString to Base Character Skills
	UFUNCTION(BlueprintCallable, Category = "Stats")
		void LoadBaseCharacterSkillsFromJSON(FString BaseCharacterSkillsJSON);

	// Character Experience Struct
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Experience")
		FHWCharacterExperience CharacterExperience;

	// Serialize Character Experience to an FString
	UFUNCTION(BlueprintCallable, Category = "Experience")
		FString SerializeCharacterExperience();

	// Deserialize an FString to Character Experience
	UFUNCTION(BlueprintCallable, Category = "Experience")
		void LoadCharacterExperienceFromJSON(FString CharacterExperienceJSON);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupURO();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
