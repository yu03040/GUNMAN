// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_PaseMenu.generated.h"

/**
 *
 */
UCLASS()
class GUNMAN_API UUI_PaseMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** ポーズメニューのテキスト */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> PoseMenu_TextBlock;

	/** タイトルへ戻るボタン */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> BackToTitle_Button;

	/** タイトルへ戻るボタンのテキスト */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> BackToTitle_TextBlock;

	/** キャンセルボタン */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> Cancel_Button;

	/** キャンセルボタンのテキスト */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Cancel_TextBlock;

	/** ゲーム終了ボタン */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> GameEnd_Button;

	/** ゲーム終了ボタンのテキスト */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> GameEnd_TextBlock;

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	UButton* GetBackToTitle_Button() const { return BackToTitle_Button; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	UButton* GetCancel_Button() const { return Cancel_Button; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	UButton* GetGameEnd_Button() const { return GameEnd_Button; }


	/** タイトルへ戻る関数 */
	UFUNCTION()
	void OnClickedBackToTitle_Button();

	/** キャンセルする関数 */
	UFUNCTION()
	void OnClickedCancel_Button();

	/** ゲームを終了する関数 */
	UFUNCTION()
	void OnClickedGameEnd_Button();
};
