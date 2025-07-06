// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "BaseMapScript.h"
#include "BattleMapScript.generated.h"

/**
 *
 */
UCLASS()
class GUNMAN_API ABattleMapScript : public ABaseMapScript
{
	GENERATED_BODY()

private:
	/* Up Input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> UpAction;

	/* Down Input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DownAction;

	/* Decision Input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DecisionAction;

protected:
	/* UI_PaseMenu の参照 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UUI_PaseMenu> UI_PaseMenu;

public:
	/* MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	TObjectPtr<class UInputMappingContext> PauseMenuMappingContext;

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	ABattleMapScript();

	// ボタンの選択状態から非選択状態に遷移するときのボタンの色を変更する関数
	virtual void ChangeButtonColor() override;

	// 選択されたボタンによって出力を変える関数
	virtual void UpdateOutputButton() override;

	// ボタンの位置を初期化する関数
	void InitializeButtonPosition();

	/**
	* ポーズメニューを表示する関数
	* @param PlayerController プレイヤーコントローラーの参照
	*/
	void DisplayPauseMenu(TObjectPtr<APlayerController>& PlayerController);
};
