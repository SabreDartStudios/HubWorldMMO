// Copyright 2022 Sabre Dart Studios


#include "HWHUD.h"

AHWHUD::AHWHUD()
{
	
}

void AHWHUD::NormalAbility1CooldownBegin_Implementation()
{
	NormalAbility1CooldownBegin();
}

void AHWHUD::NormalAbility1CooldownEnd_Implementation()
{
	NormalAbility1CooldownEnd();
}

void AHWHUD::AddFloatingDamageItem(FString DamageText, AActor* DamagedActor, FVector DamagedActorOffset, bool IsHealing, bool IsCritical, bool ShowDropShadow, bool ShowOutline)
{
	FHWFloatingDamage TempFloatingDamageItem;

	TempFloatingDamageItem.DamageText = DamageText;
	TempFloatingDamageItem.IsHealing = IsHealing;
	TempFloatingDamageItem.IsCritical = IsCritical;
	TempFloatingDamageItem.DamageTextLength = 0.f;
	TempFloatingDamageItem.DamagedActor = DamagedActor;
	TempFloatingDamageItem.DamagedActorOffset = DamagedActorOffset;
	TempFloatingDamageItem.Alpha = 1.f;
	TempFloatingDamageItem.MarkedForDeletion = false;
	TempFloatingDamageItem.ShowDropShadow = ShowDropShadow;
	TempFloatingDamageItem.ShowOutline = ShowOutline;

	if (!IsHealing) //Damage
	{
		TempFloatingDamageItem.TimeLeft = FloatingDamageMinimumDisplayTime;
	}
	else //Healing
	{
		TempFloatingDamageItem.TimeLeft = FloatingHealingMinimumDisplayTime;
	}

	FloatingDamageItems.Add(TempFloatingDamageItem);
}

void AHWHUD::CleanUpFloatingDamageItems()
{
	FloatingDamageItems.RemoveAll([](const FHWFloatingDamage item) {
		return item.MarkedForDeletion == true;
		});
}

void AHWHUD::RenderFloatingDamage(float DeltaTime)
{
	for (FHWFloatingDamage& FloatingDamageItem : FloatingDamageItems)
	{
		if (FloatingDamageItem.DamagedActor)
		{
			//Calculate text width the first time
			if (FloatingDamageItem.DamageTextLength <= 0.f)
			{
				float TextWidth = 0.f, TextHeight = 0.f;
				GetTextSize(FloatingDamageItem.DamageText, TextWidth, TextHeight, FloatingDamageFont, 1.f);
				FloatingDamageItem.DamageTextLength = TextWidth;
			}

			//Move text up
			FloatingDamageItem.DamagedActorOffset.Z += FloatingDamageSpeed.Y * DeltaTime;

			//Calcualte DamagedActor location on screen
			FVector DamageLocation = FloatingDamageItem.DamagedActor->GetActorLocation() + FloatingDamageItem.DamagedActorOffset;
			FVector ProjectedScreenLocation = Project(DamageLocation);
			FloatingDamageItem.DisplayLocation.X = ProjectedScreenLocation.X - (FloatingDamageItem.DamageTextLength / 2.f);
			FloatingDamageItem.DisplayLocation.Y = ProjectedScreenLocation.Y;

			//Remove DeltaTime from TimeLeft
			FloatingDamageItem.TimeLeft -= DeltaTime;

			//Has time expired?
			if (FloatingDamageItem.TimeLeft < 0.1f)
			{
				//Mark for deletion that will happen in CleanUpFloatingDamageItems()
				FloatingDamageItem.MarkedForDeletion = true;
			}
			else
			{
				UFont* FloatingTextFont;
				UFont* FloatingTextOutlineFont;
				FLinearColor TextColorToRender;
				FLinearColor DropShadowColorToRender;
				FLinearColor OutlineColorToRender;
				float DropShadowOffsetX;
				float DropShadowOffsetY;

				//Is this damage or healing?
				if (!FloatingDamageItem.IsHealing) //Damage
				{
					if (!FloatingDamageItem.IsCritical)
					{
						FloatingTextFont = FloatingDamageFont;
						FloatingTextOutlineFont = FloatingDamageOutlineFont;

						TextColorToRender = FloatingDamageFontColor;
						DropShadowColorToRender = FloatingDamageDropShadowColor;
						OutlineColorToRender = FloatingDamageOutlineColor;

						DropShadowOffsetX = FloatingDamageDropShadowOffsetX;
						DropShadowOffsetY = FloatingDamageDropShadowOffsetY;

						//Fade out if FloatingDamageFadeOutSpeed > 0.f
						if (FloatingDamageFadeOutSpeed > 0.f)
						{
							FloatingDamageItem.Alpha = FMath::Clamp(FloatingDamageItem.Alpha - ((FloatingDamageFadeOutSpeed / FloatingDamageMinimumDisplayTime) * DeltaTime), 0.f, 1.f);
						}
					}
					else
					{
						FloatingTextFont = FloatingCriticalDamageFont;
						FloatingTextOutlineFont = FloatingCriticalDamageOutlineFont;

						TextColorToRender = FloatingCriticalDamageFontColor;
						DropShadowColorToRender = FloatingCriticalDamageDropShadowColor;
						OutlineColorToRender = FloatingCriticalDamageOutlineColor;

						DropShadowOffsetX = FloatingCriticalDamageDropShadowOffsetX;
						DropShadowOffsetY = FloatingCriticalDamageDropShadowOffsetY;

						//Fade out if FloatingCriticalDamageFadeOutSpeed > 0.f
						if (FloatingCriticalDamageFadeOutSpeed > 0.f)
						{
							FloatingDamageItem.Alpha = FMath::Clamp(FloatingDamageItem.Alpha - ((FloatingCriticalDamageFadeOutSpeed / FloatingCriticalDamageMinimumDisplayTime) * DeltaTime), 0.f, 1.f);
						}
					}
				}
				else //Healing
				{
					if (!FloatingDamageItem.IsCritical)
					{
						FloatingTextFont = FloatingHealingFont;
						FloatingTextOutlineFont = FloatingHealingOutlineFont;

						TextColorToRender = FloatingHealingFontColor;
						DropShadowColorToRender = FloatingHealingDropShadowColor;
						OutlineColorToRender = FloatingHealingOutlineColor;

						DropShadowOffsetX = FloatingHealingDropShadowOffsetX;
						DropShadowOffsetY = FloatingHealingDropShadowOffsetY;

						//Fade out if FloatingHealingFadeOutSpeed > 0.f
						if (FloatingHealingFadeOutSpeed > 0.f)
						{
							FloatingDamageItem.Alpha = FMath::Clamp(FloatingDamageItem.Alpha - ((FloatingHealingFadeOutSpeed / FloatingHealingMinimumDisplayTime) * DeltaTime), 0.f, 1.f);
						}
					}
					else
					{
						FloatingTextFont = FloatingCriticalHealingFont;
						FloatingTextOutlineFont = FloatingCriticalHealingOutlineFont;

						TextColorToRender = FloatingCriticalHealingFontColor;
						DropShadowColorToRender = FloatingCriticalHealingDropShadowColor;
						OutlineColorToRender = FloatingCriticalHealingOutlineColor;

						DropShadowOffsetX = FloatingCriticalHealingDropShadowOffsetX;
						DropShadowOffsetY = FloatingCriticalHealingDropShadowOffsetY;

						//Fade out if FloatingCriticalHealingFadeOutSpeed > 0.f
						if (FloatingCriticalHealingFadeOutSpeed > 0.f)
						{
							FloatingDamageItem.Alpha = FMath::Clamp(FloatingDamageItem.Alpha - ((FloatingCriticalHealingFadeOutSpeed / FloatingCriticalHealingMinimumDisplayTime) * DeltaTime), 0.f, 1.f);
						}
					}
				}

				TextColorToRender.A = FloatingDamageItem.Alpha;
				DropShadowColorToRender.A = FloatingDamageItem.Alpha;
				OutlineColorToRender.A = FloatingDamageItem.Alpha;

				if (FloatingDamageItem.ShowDropShadow)
				{
					DrawText(FloatingDamageItem.DamageText, DropShadowColorToRender, FloatingDamageItem.DisplayLocation.X + DropShadowOffsetX,
						FloatingDamageItem.DisplayLocation.Y + DropShadowOffsetY, FloatingTextFont, 1.f);
				}
				if (FloatingDamageItem.ShowOutline)
				{
					DrawText(FloatingDamageItem.DamageText, OutlineColorToRender, FloatingDamageItem.DisplayLocation.X, FloatingDamageItem.DisplayLocation.Y, FloatingTextOutlineFont, 1.f);
				}
				DrawText(FloatingDamageItem.DamageText, TextColorToRender, FloatingDamageItem.DisplayLocation.X, FloatingDamageItem.DisplayLocation.Y, FloatingTextFont, 1.f);
			}
		}
	}
}