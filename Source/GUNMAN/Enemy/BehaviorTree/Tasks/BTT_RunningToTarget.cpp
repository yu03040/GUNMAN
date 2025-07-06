// Fill out your copyright notice in the Description page of Project Settings.


#include "GUNMAN/Enemy/BehaviorTree/Tasks/BTT_RunningToTarget.h"
#include "GUNMAN/Enemy/AIEnemy.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

UBTT_RunningToTarget::UBTT_RunningToTarget()
{
	// BehaviorTree に表示されるタスクノードの名前を設定
	NodeName = "RunningToTarget";
}

EBTNodeResult::Type UBTT_RunningToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TObjectPtr<UBlackboardComponent> BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent)
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
						Interface->Execute_ChangeRunningSpeed(Enemy, RunningSpeed);

						FPathFollowingRequestResult Result;
						TObjectPtr<AActor> TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKey.SelectedKeyName));
						if (TargetActor)
						{
							Result.Code = AIController->MoveToActor(TargetActor, 5.0f, false);
							if (Result.Code == EPathFollowingRequestResult::RequestSuccessful)
							{
								 Interface->Execute_ChangeRunningSpeed(Enemy, WalkSpeed);

								// タスク成功
								FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
								return EBTNodeResult::Succeeded;
							}
						}
					}
				}
			}
		}
	}
	
	// タスク失敗
	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTT_RunningToTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
					Interface->Execute_ChangeRunningSpeed(Enemy, WalkSpeed);
					FinishLatentAbort(OwnerComp);
					return EBTNodeResult::Aborted;
				}
			}
		}
	}
	return EBTNodeResult::Failed;
}


