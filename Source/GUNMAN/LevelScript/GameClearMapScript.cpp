// Fill out your copyright notice in the Description page of Project Settings.


#include "GameClearMapScript.h"
#include "GUNMAN/UMG/UIGameClear.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

AGameClearMapScript::AGameClearMapScript()
{
	UI_GameClear = NULL;
	MaxButtonCounter = 2;
	InvalidButtonIndex = 3;

	// Enhanced Input のアセットをロード
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextFinder(TEXT("/Game/GUNMAN/Input/IMC_OperatingUI.IMC_OperatingUI"));
	if (MappingContextFinder.Succeeded())
	{
		DefaultMappingContext = MappingContextFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> EnterActionFinder(TEXT("/Game/GUNMAN/Input/IA_Enter.IA_Enter"));
	if (EnterActionFinder.Succeeded())
	{
		EnterAction = EnterActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> DownArrowKeyActionFinder(TEXT("/Game/GUNMAN/Input/IA_DownArrowKey.IA_DownArrowKey"));
	if (DownArrowKeyActionFinder.Succeeded())
	{
		DownArrowKeyAction = DownArrowKeyActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> UpArrowKeyActionFinder(TEXT("/Game/GUNMAN/Input/IA_UpArrowKey.IA_UpArrowKey"));
	if (UpArrowKeyActionFinder.Succeeded())
	{
		UpArrowKeyAction = UpArrowKeyActionFinder.Object;
	}
}

void AGameClearMapScript::BeginPlay()
{
	Super::BeginPlay();

	// プレイヤーコントローラーを取得
	TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// Enhanced Input サブシステムにマッピングコンテキストを追加
	if (PlayerController)
	{
		if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	SetupInput(PlayerController->InputComponent);

	// ウィジェットブループリントのパスをセット
	FString Path = "/Game/GUNMAN/Blueprint/UMG/WBP_GameClear.WBP_GameClear_C";
	// アセットパスから UserWidgetClass を生成する
	WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*Path)).LoadSynchronous();
	// ゲームクリア用のウィジェットを作成
	if (IsValid(WidgetClass))
	{
		UI_GameClear = Cast<UUIGameClear>(CreateWidget(PlayerController, WidgetClass));

		if (UI_GameClear)
		{
			UI_GameClear->AddToViewport();
		}
	}

	//ゲームパッドで動かせるように
	PlayerController->SetInputMode(FInputModeGameOnly());

	// 最初 Continue を選択状態にする
	TObjectPtr<UButton> ContinueButton = UI_GameClear->GetContinue_Button();
	if (ContinueButton)
	{
		ContinueButton->SetBackgroundColor(SelectedColor);
	}
}

void AGameClearMapScript::SetupInput(TObjectPtr<UInputComponent> PlayerInputComponent)
{
	if (TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(EnterAction, ETriggerEvent::Triggered, this, &AGameClearMapScript::UpdateOutputButton);
		EnhancedInputComponent->BindAction(DownArrowKeyAction, ETriggerEvent::Triggered, this, &AGameClearMapScript::UI_DownwardMovement);
		EnhancedInputComponent->BindAction(UpArrowKeyAction, ETriggerEvent::Triggered, this, &AGameClearMapScript::UI_UpwardMovement);
	}
}

void AGameClearMapScript::ChangeButtonColor()
{
	// 非選択状態の色をそれぞれ白色に
	TObjectPtr<UButton> ContinueButton = UI_GameClear->GetContinue_Button();
	if (ContinueButton)
	{
		ContinueButton->SetBackgroundColor(FLinearColor::White);
	}

	TObjectPtr<UButton> EndButton = UI_GameClear->GetGameEnd_Button();
	if (EndButton)
	{
		EndButton->SetBackgroundColor(FLinearColor::White);
	}

	// 選択状態のボタンの色をそれぞれ変える
	switch (ButtonCounter)
	{
		case 1:
			ContinueButton->SetBackgroundColor(SelectedColor);
			break;
		case 2:
			EndButton->SetBackgroundColor(SelectedColor);
			break;
	}
}

void AGameClearMapScript::UpdateOutputButton()
{

	switch (ButtonCounter)
	{
		case 1:
			UI_GameClear->OnClickedContinue_Button();
			break;
		case 2:
			UI_GameClear->OnClickedGameEnd_Button();
			break;
	}

}
