// Copyright 2023 Sabre Dart Studios

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "HWAttributeSet.generated.h"

class UHWAbilitySystemComponent;
class UObject;
class UWorld;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

//Forward declare to stop circular reference
class AHWGASCharacter;

/**
 * 
 */
UCLASS()
class OWSHUBWORLDMMO_API UHWAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:

    UWorld* GetWorld() const override;
    UHWAbilitySystemComponent* GetHWAbilitySystemComponent() const;
};
