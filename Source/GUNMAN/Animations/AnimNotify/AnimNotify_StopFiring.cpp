// Fill out your copyright notice in the Description page of Project Settings.


#include "GUNMAN/Animations/AnimNotify/AnimNotify_StopFiring.h"
#include "GUNMAN/Characters/GUNMANCharacter.h"

void UAnimNotify_StopFiring::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
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
				// 発砲を止める通知
				Interface->Execute_FireState(Player, false);
			}
		}
	}
}

FString UAnimNotify_StopFiring::GetNotifyName_Implementation() const
{
	return FString(TEXT("StopFiring"));
}