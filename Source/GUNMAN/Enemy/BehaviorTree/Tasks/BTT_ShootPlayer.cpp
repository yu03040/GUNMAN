// Fill out your copyright notice in the Description page of Project Settings.


#include "GUNMAN/Enemy/BehaviorTree/Tasks/BTT_ShootPlayer.h"
#include "GUNMAN/Enemy/AIEnemy.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTT_ShootPlayer::UBTT_ShootPlayer()
{
	// BehaviorTree に表示されるタスクノードの名前を設定
	NodeName = "ShootPlayer";
}

EBTNodeResult::Type UBTT_ShootPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// AIController にアクセス
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		// ControlledPawn にアクセス
		APawn* ControlledPawn = AIController->GetPawn();
		if (ControlledPawn)
		{
			TObjectPtr<AAIEnemy> Enemy = Cast<AAIEnemy>(ControlledPawn);
			if (Enemy)
			{
				IAIEnemyInterface* Interface = Cast<IAIEnemyInterface>(Enemy);
				if (Interface)
				{
					Interface->Execute_AttackCharacter(Enemy);
				}

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}

