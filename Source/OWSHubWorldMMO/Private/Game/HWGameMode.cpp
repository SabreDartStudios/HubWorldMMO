// Copyright 2023 Sabre Dart Studios

#include "Game/HWGameMode.h"
#include "Game/HWPlayerState.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"

AHWGameMode::AHWGameMode()
{
	PlayerStateClass = AHWPlayerState::StaticClass();

	bEnableContentPullOnServerSpinUp = false;
	OnZoneContentLoaded.AddDynamic(this, &AHWGameMode::OnZoneContentLoadedHandler);
}

void AHWGameMode::OnZoneContentLoadedHandler(FString ContentJSON)
{
	ZoneNpcs.Empty();

	TSharedPtr<FJsonObject> OuterObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ContentJSON);
	if (!FJsonSerializer::Deserialize(Reader, OuterObject) || !OuterObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("HWGameMode::OnZoneContentLoadedHandler - Failed to parse ContentJSON"));
		return;
	}

	const TArray<TSharedPtr<FJsonValue>>* NpcsArray;
	if (!OuterObject->TryGetArrayField(TEXT("npcs"), NpcsArray))
	{
		NotifyZoneNpcsLoaded();
		return;
	}

	for (const TSharedPtr<FJsonValue>& NpcValue : *NpcsArray)
	{
		const TSharedPtr<FJsonObject>* NpcObject;
		if (!NpcValue->TryGetObject(NpcObject))
		{
			continue;
		}

		FNpcSpawnData NpcData;

		double TempDouble = 0.0;

		if ((*NpcObject)->TryGetNumberField(TEXT("id"), TempDouble))
			NpcData.Id = FMath::RoundToInt(TempDouble);

		(*NpcObject)->TryGetStringField(TEXT("name"), NpcData.NpcName);

		if ((*NpcObject)->TryGetNumberField(TEXT("x"), TempDouble))
			NpcData.X = (float)TempDouble;

		if ((*NpcObject)->TryGetNumberField(TEXT("y"), TempDouble))
			NpcData.Y = (float)TempDouble;

		if ((*NpcObject)->TryGetNumberField(TEXT("z"), TempDouble))
			NpcData.Z = (float)TempDouble;

		if ((*NpcObject)->TryGetNumberField(TEXT("rx"), TempDouble))
			NpcData.Rx = (float)TempDouble;

		if ((*NpcObject)->TryGetNumberField(TEXT("ry"), TempDouble))
			NpcData.Ry = (float)TempDouble;

		if ((*NpcObject)->TryGetNumberField(TEXT("rz"), TempDouble))
			NpcData.Rz = (float)TempDouble;

		ZoneNpcs.Add(NpcData);
	}

	NotifyZoneNpcsLoaded();
}
