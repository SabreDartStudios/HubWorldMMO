// Copyright 2023 Sabre Dart Studios


#include "./AbilitySystem/HWCombatDataSubsystem.h"
#include "Serialization/JsonTypes.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"


void UHWCombatDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	/*
	FCombatAbility FirstAbility;
	FirstAbility.AbilityName = "Ability1";
	FirstAbility.FloatCombatValues.Add("AttackCoefficient", 1.2f);

	CombatAbilities.Add("Ability1", FirstAbility);
	*/

	FString CombatDataJSON = "{ \"Abilities\": [{\"AbilityName\": \"Ability1\", \"FloatCombatValues\": [{ \"FloatCombatValueName\": \"AttackCoefficient\", \"FloatCombatValue\": 1.2 },{ \"FloatCombatValueName\": \"DefenseCoefficient\", \"FloatCombatValue\": 0.0 }] }] }";
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(CombatDataJSON);

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		TArray<TSharedPtr<FJsonValue>> Abilities = JsonObject->GetArrayField("Abilities");
		for (auto Ability : Abilities)
		{
			const TSharedPtr<FJsonObject>* CurrentCombatAbility;
			if (Ability->TryGetObject(CurrentCombatAbility))
			{
				FCombatAbility AbilityToAdd;
				AbilityToAdd.AbilityName = CurrentCombatAbility->Get()->GetStringField("AbilityName");
				TArray<TSharedPtr<FJsonValue>> FloatCombatValues = CurrentCombatAbility->Get()->GetArrayField("FloatCombatValues");
				for (auto FloatCombatValue : FloatCombatValues)
				{
					const TSharedPtr<FJsonObject>* CurrentCombatAbilityFloatCombatValues;
					if (FloatCombatValue->TryGetObject(CurrentCombatAbilityFloatCombatValues))
					{
						FString FloatCombatValueName = CurrentCombatAbilityFloatCombatValues->Get()->GetStringField("FloatCombatValueName");
						float FloatCombatValueFloat = CurrentCombatAbilityFloatCombatValues->Get()->GetNumberField("FloatCombatValue");
						AbilityToAdd.FloatCombatValues.Add(FloatCombatValueName, FloatCombatValueFloat);
					}
				}

				CombatAbilities.Add(AbilityToAdd.AbilityName, AbilityToAdd);
			}
		}
	}
}

void UHWCombatDataSubsystem::Deinitialize()
{
	CombatAbilities.Empty();
}

float UHWCombatDataSubsystem::GetFloatValue(FString AbilityName, FString FloatCombatValueName)
{
	FCombatAbility* FoundCombatAbility = CombatAbilities.Find(AbilityName);

	if (FoundCombatAbility)
	{
		float* FoundFloatCombatValue = FoundCombatAbility->FloatCombatValues.Find(FloatCombatValueName);

		if (FoundFloatCombatValue)
		{
			return *FoundFloatCombatValue;
		}

		return -999998.f; //Float Combat Value not found Error Value!
	}

	return -999999.f; //Ability not found Error Value!
}