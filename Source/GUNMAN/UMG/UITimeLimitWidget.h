// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UITimeLimitWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUNMAN_API UUITimeLimitWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	bool Initialize() override;

	float GetTime() const { return TimeLimitSeconds; }

	void SetTime(float time) { TimeLimitSeconds = time; }

protected:
	/** 制限時間のテキスト */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TimeLimit_TextBlock;

	/** 制限時間の秒数を表示するテキスト */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TimeRemoving_TextBlock;

	/** 制限時間の秒数 */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float TimeLimitSeconds = 120.0f;

	/** 制限時間を更新する関数 */
	UFUNCTION()
	FText SetTextBlockUpdateTimeLimit();
};
