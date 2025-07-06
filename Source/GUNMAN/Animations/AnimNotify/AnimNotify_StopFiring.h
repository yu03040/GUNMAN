// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_StopFiring.generated.h"

/**
 * 
 */
UCLASS()
class GUNMAN_API UAnimNotify_StopFiring : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	// 表示文字列
	virtual FString GetNotifyName_Implementation() const override;
};
