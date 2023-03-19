// Copyright 2022 Sabre Dart Studios


#include "./Login/HWLoginPlayerController.h"

AHWLoginPlayerController::AHWLoginPlayerController()
{
	//Create UOWSPlayerControllerComponent and bind delegates
	OWSPlayerControllerComponent = CreateDefaultSubobject<UOWSPlayerControllerComponent>(TEXT("OWS Player Controller Component"));

	OWSPlayerControllerComponent->OnNotifyGetAllCharactersDelegate.BindUObject(this, &AHWLoginPlayerController::NotifyGetAllCharacters);
	OWSPlayerControllerComponent->OnErrorGetAllCharactersDelegate.BindUObject(this, &AHWLoginPlayerController::ErrorGetAllCharacters);
	OWSPlayerControllerComponent->OnNotifyPlayerLogoutDelegate.BindUObject(this, &AHWLoginPlayerController::NotifyPlayerLogout);
	OWSPlayerControllerComponent->OnErrorPlayerLogoutDelegate.BindUObject(this, &AHWLoginPlayerController::ErrorPlayerLogout);
	OWSPlayerControllerComponent->OnNotifyCreateCharacterUsingDefaultCharacterValuesDelegate.BindUObject(this, &AHWLoginPlayerController::NotifyCreateCharacterUsingDefaultCharacterValues);
	OWSPlayerControllerComponent->OnErrorCreateCharacterUsingDefaultCharacterValuesDelegate.BindUObject(this, &AHWLoginPlayerController::ErrorCreateCharacterUsingDefaultCharacterValues);

}

void AHWLoginPlayerController::NotifyGetCharacterDataAndCustomData2(TSharedPtr<FJsonObject> JsonObject)
{
	TArray<FCustomCharacterDataStruct> CustomData;

	if (JsonObject->HasField("CustomCharacterDataRows"))
	{
		TArray<TSharedPtr<FJsonValue>> Rows = JsonObject->GetArrayField("CustomCharacterDataRows");

		for (int RowNum = 0; RowNum != Rows.Num(); RowNum++) {
			FCustomCharacterDataStruct tempCustomData;
			TSharedPtr<FJsonObject> tempRow = Rows[RowNum]->AsObject();
			tempCustomData.CustomFieldName = tempRow->GetStringField("CustomFieldName");
			tempCustomData.FieldValue = tempRow->GetStringField("FieldValue");

			CustomData.Add(tempCustomData);
		}
	}

	NotifyGetCharacterDataAndCustomData(CustomData);
}