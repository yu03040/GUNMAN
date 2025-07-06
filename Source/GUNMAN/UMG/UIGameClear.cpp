// Fill out your copyright notice in the Description page of Project Settings.

#include "UIGameClear.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GamePlayStatics.h"

void UUIGameClear::OnClickedContinue_Button()
{
	// コンティニューする
	UGameplayStatics::OpenLevel(this, "TitleMap");
}

void UUIGameClear::OnClickedGameEnd_Button()
{
	// ゲーム終了
	UKismetSystemLibrary::QuitGame(this, NULL, EQuitPreference::Quit, false);
}