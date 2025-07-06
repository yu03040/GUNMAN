// Fill out your copyright notice in the Description page of Project Settings.

#include "../UMG/UITitle.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/InputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GamePlayStatics.h"

void UUITitle::OnClickedGameStart_Button()
{
	// ゲーム開始
	UGameplayStatics::OpenLevel(this, "BattleMap");
}

void UUITitle::OnClickedGameEnd_Button()
{
	// ゲーム終了
	UKismetSystemLibrary::QuitGame(this, NULL, EQuitPreference::Quit, false);
}

void UUITitle::OnClickedGameStartExplaination_Button()
{
	Explaination_CanvasPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UUITitle::OnClickedGameEndExplaination_Button()
{
	Explaination_CanvasPanel->SetVisibility(ESlateVisibility::Hidden);
}