// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMapScript.h"

ABaseMapScript::ABaseMapScript()
{
	HasMovedToInstructions = false;
	ButtonCounter = 1;
}

void ABaseMapScript::UI_UpwardMovement()
{
	// 操作説明のキャンバスにいるときは動かないようにする
	if (HasMovedToInstructions)
	{
	}
	else
	{
		// 上にいくほどインデックスは小さくする
		ButtonCounter -= 1;

		// 0 以下になったら、一番下の UI を選択する
		if (ButtonCounter <= 0)
		{
			ButtonCounter = MaxButtonCounter;
			ChangeButtonColor();
		}
		else
		{
			ChangeButtonColor();
		}
	}
}

void ABaseMapScript::UI_DownwardMovement()
{
	// 操作説明のキャンバスにいるときは動かないようにする
	if (HasMovedToInstructions)
	{
	}
	else
	{
		// 下にいくほどインデックスは大きくする
		ButtonCounter += 1;

		// 4 以上になったら、一番上の UI を選択する
		if (ButtonCounter >= InvalidButtonIndex)
		{
			ButtonCounter = 1;
		}
		ChangeButtonColor();
	}
}

void ABaseMapScript::ChangeButtonColor()
{
}

void ABaseMapScript::UpdateOutputButton()
{
}

void ABaseMapScript::BeginPlay()
{
	Super::BeginPlay();
}
