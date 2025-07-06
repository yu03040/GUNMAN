// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GUNMANGameModeBase.h"
#include "GUNMAN/Enemy/AIEnemy.h"
#include "GUNMAN/Characters/GUNMANCharacter.h"
#include "GUNMAN/UMG/UITimeLimitWidget.h"
#include "GUNMANGameMode.generated.h"

UCLASS(minimalapi)
class AGUNMANGameMode : public AGUNMANGameModeBase
{
	GENERATED_BODY()

private:
	/** ゲームクリアに必要な倒した数 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	int GameClearKillCount = 10;

	/** ゲームオーバーの時間 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	float GameOverTime = 0.0f;

	/** ゲームクリア後の待機時間 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	float GameClearWaitingTime = 2.0f;

	/** UITimeLimitWidget の 参照 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UUITimeLimitWidget> UITimeLimitRef;

	/* ブループリントのウィジェットクラス */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> WidgetClass;

protected:
	virtual void BeginPlay() override;

public:
	AGUNMANGameMode();

	virtual void Tick(float DeltaTime) override;

	// 制限時間のウィジェットを表示する関数
	void DisplayTimeLimit();

	// GameClearMap を開く関数
	void OpenGameClearMap();

	// GameOverMap を開く関数
	void OpenGameOverMap();
};