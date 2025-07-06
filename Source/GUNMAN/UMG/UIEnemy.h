// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GUNMAN_API UUIEnemy : public UUserWidget
{
	GENERATED_BODY()

public:
	bool Initialize() override;

	/* 体力を表示する関数 */
	UFUNCTION()
	float SetHealthProgressBar();

	void SetOwningEnemy(TObjectPtr<class AAIEnemy> NewOwner);

protected:
	/** 体力バー */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UProgressBar> Health_ProgressBar;

	UPROPERTY()
	TObjectPtr<AAIEnemy> OwningEnemy;
};
