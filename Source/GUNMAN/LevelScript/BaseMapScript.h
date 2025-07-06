// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "BaseMapScript.generated.h"

/**
 *
 */
UCLASS()
class GUNMAN_API ABaseMapScript : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	ABaseMapScript();

	/* コントローラーでUIを上に移動できる関数 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UI_UpwardMovement();

	/* コントローラーでUIを下に移動できる関数 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UI_DownwardMovement();

	/* ボタンの選択状態から非選択状態に遷移するときのボタンの色を変更する関数 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void ChangeButtonColor();

	/* 選択されたボタンによって出力を変える関数 */
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void UpdateOutputButton();

protected:
	virtual void BeginPlay() override;

	/* ブループリントのウィジェットクラス */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> WidgetClass;

	/* ボタンが選択されているときの色 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FLinearColor SelectedColor;

	/* 操作説明のキャンバスに移動したか？ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	bool HasMovedToInstructions;

	/* 選択しているボタンのインデックス */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	int ButtonCounter;

	/* 選択しているボタンのインデックスの最大値 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	int MaxButtonCounter;

	/* 選択しているボタンのインデックスの範囲外の値 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	int InvalidButtonIndex;
};
