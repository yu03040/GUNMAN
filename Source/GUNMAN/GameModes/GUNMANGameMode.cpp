// Copyright Epic Games, Inc. All Rights Reserved.

#include "GUNMANGameMode.h"
#include "GUNMAN/Characters/GUNMANCharacter.h"
#include "GUNMAN/Controllers/GUNMANController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "GUNMAN/UMG/UITimeLimitWidget.h"
#include "UObject/ConstructorHelpers.h"

AGUNMANGameMode::AGUNMANGameMode() : Super()
{
	// 毎フレームTick()を呼び出すように設定
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/GUNMAN/Blueprint/Characters/ThirdPersonCharacter.ThirdPersonCharacter_C"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerControllerClass = AGUNMANController::StaticClass();
}

void AGUNMANGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 制限時間のウィジェットを表示する
	DisplayTimeLimit();
}

void AGUNMANGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 予め決めた制限時間から経過時間を引く
	UITimeLimitRef->SetTime(UITimeLimitRef->GetTime() - DeltaTime);

	// 0 秒以下ならゲーム終了（ゲームオーバー）
	if (UITimeLimitRef->GetTime() <= GameOverTime)
	{
		OpenGameOverMap();
	}

	TObjectPtr<AGUNMANCharacter> Player = Cast<AGUNMANCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (Player)
	{
		// 1敵を10体以上倒したらゲームクリア
		if (Player->GetKillCount() >= GameClearKillCount)
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGUNMANGameMode::OpenGameClearMap, GameClearWaitingTime);
		}
	}
}

void AGUNMANGameMode::DisplayTimeLimit()
{
	// WidgetBlueprint の Class を取得する
	FString Path = "/Game/GUNMAN/Blueprint/UMG/WBP_TimeLimit.WBP_TimeLimit_C";
	WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*Path)).LoadSynchronous();

	// プレイヤーコントローラーを取得する
	TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	// WidgetClass と PlayerController が取得できたか判定する
	if (WidgetClass && PlayerController)
	{
		// 制限時間用のウィジェットを作成
		UITimeLimitRef = Cast<UUITimeLimitWidget>(CreateWidget(PlayerController, WidgetClass));

		if (UITimeLimitRef)
		{
			// 制限時間を画面に表示する
			UITimeLimitRef->AddToViewport();
		}
	}
}

void AGUNMANGameMode::OpenGameClearMap()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("GameClearMap")));
}

void AGUNMANGameMode::OpenGameOverMap()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("GameOverMap")));
}