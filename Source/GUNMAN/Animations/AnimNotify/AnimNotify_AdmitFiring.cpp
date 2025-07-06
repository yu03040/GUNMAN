// Fill out your copyright notice in the Description page of Project Settings.


#include "GUNMAN/Animations/AnimNotify/AnimNotify_AdmitFiring.h"
#include "GUNMAN/Characters/GUNMANCharacter.h"

void UAnimNotify_AdmitFiring::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (MeshComp)
	{
		TObjectPtr<AGUNMANCharacter> Player;
		Player = Cast<AGUNMANCharacter>(MeshComp->GetOwner());
		if (Player)
		{
			IAnimationInterface* Interface = Cast<IAnimationInterface>(Player);
			if (Interface)
			{
				// 発砲を認める通知
				Interface->Execute_FireState(Player, true);
			}
		}
	}
}

FString UAnimNotify_AdmitFiring::GetNotifyName_Implementation() const
{
	return FString(TEXT("AdmitFiring"));
}
