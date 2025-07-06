// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIGameClear.generated.h"

/**
 *
 */
UCLASS()
class GUNMAN_API UUIGameClear : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** コンティニューボタン */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> Continue_Button;

	/** コンティニューボタンのテキスト */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Continue_TextBlock;

	/** ゲーム終了ボタン */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> GameEnd_Button;

	/** ゲーム終了ボタンのテキスト */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> GameEnd_TextBlock;

public:
	/** ゲームを終了する関数 */
	UFUNCTION()
	void OnClickedContinue_Button();

	/** ゲームを終了する関数 */
	UFUNCTION()
	void OnClickedGameEnd_Button();

	UFUNCTION(BlueprintPure, Category = "Button")
	UButton* GetContinue_Button() const { return Continue_Button; }

	UFUNCTION(BlueprintPure, Category = "Button")
	UButton* GetGameEnd_Button() const { return GameEnd_Button; }
};
