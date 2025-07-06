// Fill out your copyright notice in the Description page of Project Settings.

#include "UI_PaseMenu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GamePlayStatics.h"

void UUI_PaseMenu::OnClickedBackToTitle_Button()
{
	// タイトルへ戻る
	UGameplayStatics::OpenLevel(this, "TitleMap");
}

void UUI_PaseMenu::OnClickedCancel_Button()
{
	RemoveFromParent();
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void UUI_PaseMenu::OnClickedGameEnd_Button()
{
	// ゲーム終了
	UKismetSystemLibrary::QuitGame(this, NULL, EQuitPreference::Quit, false);
}
