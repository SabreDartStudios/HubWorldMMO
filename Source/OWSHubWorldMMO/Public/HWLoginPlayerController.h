// Copyright 2022 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "OWSPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "HWLoginPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API AHWLoginPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AHWLoginPlayerController();

	UPROPERTY(BlueprintReadOnly)
		UOWSPlayerControllerComponent* OWSPlayerControllerComponent;

	UFUNCTION(BlueprintImplementableEvent, Category = "Login")
		void NotifyGetAllCharacters(const TArray<FUserCharacter>& UserCharacters);
	UFUNCTION(BlueprintImplementableEvent, Category = "Login")
		void ErrorGetAllCharacters(const FString& ErrorMsg);

	UFUNCTION(BlueprintImplementableEvent, Category = "Login")
		void NotifyPlayerLogout();
	UFUNCTION(BlueprintImplementableEvent, Category = "Login")
		void ErrorPlayerLogout(const FString& ErrorMsg);

	UFUNCTION(BlueprintImplementableEvent, Category = "Login")
		void NotifyCreateCharacter(const FCreateCharacter& CreateCharacterData);
	UFUNCTION(BlueprintImplementableEvent, Category = "Login")
		void ErrorCreateCharacter(const FString& ErrorMsg);

	UFUNCTION(BlueprintImplementableEvent, Category = "Character")
		void NotifyGetCharacterDataAndCustomData(const TArray<FCustomCharacterDataStruct>& CustomCharacterData);
	UFUNCTION(BlueprintImplementableEvent, Category = "Character")
		void ErrorGetCharacterDataAndCustomData(const FString& ErrorMsg);

private:
	void NotifyGetCharacterDataAndCustomData2(TSharedPtr<FJsonObject> JsonObject);
};
