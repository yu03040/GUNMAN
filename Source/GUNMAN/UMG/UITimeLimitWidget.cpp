// Fill out your copyright notice in the Description page of Project Settings.

#include "UITimeLimitWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetTextLibrary.h"

bool UUITimeLimitWidget::Initialize()
{
	bool Success = Super::Initialize();

	if (Success == false)
	{
		return false;
	}

	TimeRemoving_TextBlock->TextDelegate.BindUFunction(this, "SetTextBlockUpdateTimeLimit");

	return true;
}

FText UUITimeLimitWidget::SetTextBlockUpdateTimeLimit()
{
	return UKismetTextLibrary::Conv_IntToText(UKismetMathLibrary::Round(TimeLimitSeconds));
}

