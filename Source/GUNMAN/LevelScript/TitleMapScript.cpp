// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleMapScript.h"
#include "GUNMAN/UMG/UITitle.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

ATitleMapScript::ATitleMapScript()
{
	UI_Title = NULL;
	MaxButtonCounter = 3;
	InvalidButtonIndex = 4;

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

void ATitleMapScript::BeginPlay()
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
	FString Path = "/Game/GUNMAN/Blueprint/UMG/WBP_Title.WBP_Title_C";
	// アセットパスから UserWidgetClass を生成する
	WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*Path)).LoadSynchronous();
	// タイトル用のウィジェットを作成
	if (IsValid(WidgetClass))
	{
		UI_Title = Cast<UUITitle>(CreateWidget(PlayerController, WidgetClass));

		if (UI_Title)
		{
			UI_Title->AddToViewport();
		}
	}

	//ゲームパッドで動かせるように
	PlayerController->SetInputMode(FInputModeGameOnly());

	// 最初 Game Start を選択状態にする
	TObjectPtr<UButton> StartButton = UI_Title->GetGameStart_Button();
	if (StartButton)
	{
		StartButton->SetBackgroundColor(SelectedColor);
	}
}

void ATitleMapScript::SetupInput(TObjectPtr<UInputComponent> PlayerInputComponent)
{
	if (TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(EnterAction, ETriggerEvent::Triggered, this, &ATitleMapScript::UpdateOutputButton);
		EnhancedInputComponent->BindAction(DownArrowKeyAction, ETriggerEvent::Triggered, this, &ATitleMapScript::UI_DownwardMovement);
		EnhancedInputComponent->BindAction(UpArrowKeyAction, ETriggerEvent::Triggered, this, &ATitleMapScript::UI_UpwardMovement);
	}
}

void ATitleMapScript::ChangeButtonColor()
{
	// 非選択状態の色をそれぞれ白色に
	TObjectPtr<UButton> StartButton = UI_Title->GetGameStart_Button();
	if (StartButton)
	{
		StartButton->SetBackgroundColor(FLinearColor::White);
	}

	TObjectPtr<UButton> EndButton = UI_Title->GetGameEnd_Button();
	if (EndButton)
	{
		EndButton->SetBackgroundColor(FLinearColor::White);
	}

	TObjectPtr<UButton> StartTutorialButton = UI_Title->GetGameStartExplaination_Button();
	if (StartTutorialButton)
	{
		StartTutorialButton->SetBackgroundColor(FLinearColor::White);
	}

	// 選択状態のボタンの色をそれぞれ変える
	switch (ButtonCounter)
	{
		case 1:
			StartButton->SetBackgroundColor(SelectedColor);
			break;
		case 2:
			EndButton->SetBackgroundColor(SelectedColor);
			break;
		case 3:
			StartTutorialButton->SetBackgroundColor(SelectedColor);
			break;
	}
}

void ATitleMapScript::UpdateOutputButton()
{
	// 操作説明のキャンバスにいるか
	if (HasMovedToInstructions)
	{
		// 操作説明のキャンバスを閉じる
		UI_Title->OnClickedGameEndExplaination_Button();
		HasMovedToInstructions = false;
	}
	else
	{
		switch (ButtonCounter)
		{
			case 1:
				UI_Title->OnClickedGameStart_Button();
				break;
			case 2:
				UI_Title->OnClickedGameEnd_Button();
				break;
			case 3:
				// 操作説明のキャンバスを閉じるボタンの色を変える
				UI_Title->OnClickedGameStartExplaination_Button();
				TObjectPtr<UButton> EndTutorialButton = UI_Title->GetGameEndExplaination_Button();
				EndTutorialButton->SetBackgroundColor(SelectedColor);
				HasMovedToInstructions = true;
				break;
		}
	}
}