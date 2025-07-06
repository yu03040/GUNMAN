// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UITitle.generated.h"

/**
 *
 */
UCLASS()
class GUNMAN_API UUITitle : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Button")
	UButton* GetGameStart_Button() const { return GameStart_Button; }

	UFUNCTION(BlueprintPure, Category = "Button")
	UButton* GetGameEnd_Button() const { return GameEnd_Button; }

	UFUNCTION(BlueprintPure, Category = "Button")
	UButton* GetGameStartExplaination_Button() const { return GameStartExplaination_Button; }

	UFUNCTION(BlueprintPure, Category = "Button")
	UButton* GetGameEndExplaination_Button() const { return GameEndExplaination_Button; }

	/** ゲームを開始する関数 */
	UFUNCTION(BlueprintCallable)
	void OnClickedGameStart_Button();

	/** ゲームを終了する関数 */
	UFUNCTION(BlueprintCallable)
	void OnClickedGameEnd_Button();

	/** 操作説明を開始する関数 */
	UFUNCTION(BlueprintCallable)
	void OnClickedGameStartExplaination_Button();

	/** 操作説明を終了する関数 */
	UFUNCTION(BlueprintCallable)
	void OnClickedGameEndExplaination_Button();

protected:
	/** ゲーム開始ボタン */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> GameStart_Button;

	/** ゲーム開始ボタンのテキスト */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> GameStart_TextBlock;

	/** ゲーム終了ボタン */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> GameEnd_Button;

	/** ゲーム終了ボタンのテキスト */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> GameEnd_TextBlock;

	/** 操作説明開始ボタン */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> GameStartExplaination_Button;

	/** 操作説明終了ボタン */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> GameEndExplaination_Button;

	/** 操作説明のキャンバス*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> Explaination_CanvasPanel;
};
