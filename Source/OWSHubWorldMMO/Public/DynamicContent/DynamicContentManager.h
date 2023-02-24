// Copyright 2023 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "DynamicContentManager.generated.h"

UCLASS()
class OWSHUBWORLDMMO_API ADynamicContentManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADynamicContentManager();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactable")
		UDataTable* InteractablesToSpawn;

	void SpawnInteractables();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
