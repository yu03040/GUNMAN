// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleMapScript.h"
#include "GUNMAN/UMG/UI_PaseMenu.h"
#include "GUNMAN/UMG/UICharacter.h"
#include "GUNMAN/Characters/GUNMANCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

ABattleMapScript::ABattleMapScript()
{
	UI_PaseMenu = NULL;
	MaxButtonCounter = 3;
	InvalidButtonIndex = 4;

	// Enhanced Input のアセットをロード
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextFinder(TEXT("/Game/GUNMAN/Input/IMC_OperatingUI.IMC_OperatingUI"));
	if (MappingContextFinder.Succeeded())
	{
		PauseMenuMappingContext = MappingContextFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> UpActionFinder(TEXT("/Game/GUNMAN/Input/IA_UpArrowKey.IA_UpArrowKey"));
	if (UpActionFinder.Succeeded())
	{
		UpAction = UpActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> DownActionFinder(TEXT("/Game/GUNMAN/Input/IA_DownArrowKey.IA_DownArrowKey"));
	if (DownActionFinder.Succeeded())
	{
		DownAction = DownActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> DecisionActionFinder(TEXT("/Game/GUNMAN/Input/IA_Enter.IA_Enter"));
	if (DecisionActionFinder.Succeeded())
	{
		DecisionAction = DecisionActionFinder.Object;
	}
}

void ABattleMapScript::BeginPlay()
{
	Super::BeginPlay();

	// プレイヤーコントローラーを取得
	TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// Enhanced Input サブシステムにマッピングコンテキストを追加
	if (PlayerController)
	{
		if (TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(UpAction, ETriggerEvent::Triggered, this, &ABattleMapScript::UI_UpwardMovement);
			EnhancedInputComponent->BindAction(DownAction, ETriggerEvent::Triggered, this, &ABattleMapScript::UI_DownwardMovement);
			EnhancedInputComponent->BindAction(DecisionAction, ETriggerEvent::Triggered, this, &ABattleMapScript::UpdateOutputButton);
		}
	}
}

void ABattleMapScript::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// プレイヤーコントローラーを取得
	TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// Enhanced Input サブシステムにマッピングコンテキストを追加
	if (PlayerController)
	{
		if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(PauseMenuMappingContext);
		}
	}
}

void ABattleMapScript::ChangeButtonColor()
{
	// 非選択状態の色をそれぞれ白色に
	TObjectPtr<UButton> BackToTitleButton = UI_PaseMenu->GetBackToTitle_Button();
	if (BackToTitleButton)
	{
		BackToTitleButton->SetBackgroundColor(FLinearColor::White);
	}

	TObjectPtr<UButton> CancelButton = UI_PaseMenu->GetCancel_Button();
	if (CancelButton)
	{
		CancelButton->SetBackgroundColor(FLinearColor::White);
	}

	TObjectPtr<UButton> EndButton = UI_PaseMenu->GetGameEnd_Button();
	if (EndButton)
	{
		EndButton->SetBackgroundColor(FLinearColor::White);
	}

	// 選択状態のボタンの色をそれぞれ変える
	switch (ButtonCounter)
	{
		case 1:
			BackToTitleButton->SetBackgroundColor(SelectedColor);
			break;
		case 2:
			CancelButton->SetBackgroundColor(SelectedColor);
			break;
		case 3:
			EndButton->SetBackgroundColor(SelectedColor);
			break;
	}
}

void ABattleMapScript::UpdateOutputButton()
{
	switch (ButtonCounter)
	{
		case 1:
			UI_PaseMenu->OnClickedBackToTitle_Button();
			break;
		case 2:
		{
			UI_PaseMenu->OnClickedCancel_Button();
			// プレイヤーコントローラーを取得
			TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(this, 0);

			if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(PauseMenuMappingContext);
				TObjectPtr<AGUNMANCharacter> GunmanCharacter = Cast<AGUNMANCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
				Subsystem->AddMappingContext(GunmanCharacter->DefaultMappingContext, 0);
			}
			// ボタンの位置を初期化
			ButtonCounter = 1;
			break;
		}
		case 3:
			UI_PaseMenu->OnClickedGameEnd_Button();
			break;

	}
}

void ABattleMapScript::InitializeButtonPosition()
{
	// PlayerController を取得
	TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	//ゲームを止めて、UI をゲームパッドで動かす
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	PlayerController->SetInputMode(FInputModeGameOnly());

	// ポーズメニューを表示する
	DisplayPauseMenu(PlayerController);

	// 最初 Back to Title を選択状態にする
	TObjectPtr<UButton> BackToTitleButton = UI_PaseMenu->GetBackToTitle_Button();
	if (BackToTitleButton)
	{
		BackToTitleButton->SetBackgroundColor(SelectedColor);
	}
}

void ABattleMapScript::DisplayPauseMenu(TObjectPtr<APlayerController>& PlayerController)
{
	// WidgetBlueprint の Class を取得する
	FString Path = "/Game/GUNMAN/Blueprint/UMG/WBP_PaseMenu.WBP_PaseMenu_C";
	WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*Path)).LoadSynchronous();

	if (WidgetClass && PlayerController)
	{
		// ポーズメニュー用のウィジェットを生成
		UI_PaseMenu = Cast<UUI_PaseMenu>(CreateWidget(PlayerController, WidgetClass));
		if (UI_PaseMenu)
		{
			// ポーズメニューを画面に表示する
			UI_PaseMenu->AddToViewport();
		}
	}
}
