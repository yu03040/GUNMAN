// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMapScript.h"
#include "GameOverMapScript.generated.h"

/**
 *
 */
UCLASS()
class GUNMAN_API AGameOverMapScript : public ABaseMapScript
{
	GENERATED_BODY()

public:
	AGameOverMapScript();

	/* ボタンの選択状態から非選択状態に遷移するときのボタンの色を変更する関数 */
	virtual void ChangeButtonColor() override;

	/* 選択されたボタンによって出力を変える関数 */
	virtual void UpdateOutputButton() override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInput(TObjectPtr<UInputComponent> PlayerInputComponent);

private:
	/* UIGameClear の Ref */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UUIGameOver> UI_GameOver;

	/* MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	/* Enter Input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> EnterAction;

	/* Down Arrow Key Input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DownArrowKeyAction;

	/* Up Arrow Key Input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> UpArrowKeyAction;
};
