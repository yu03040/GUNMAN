// Fill out your copyright notice in the Description page of Project Settings.


#include "GUNMAN/Enemy/BehaviorTree/Decorators/BTD_FarFromTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "AIController.h"

UBTD_FarFromTarget::UBTD_FarFromTarget()
{
	// BehaviorTree に表示されるタスクノードの名前を設定
	NodeName = "FarFromTarget";
}

bool UBTD_FarFromTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	TObjectPtr<UBlackboardComponent> BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent)
	{
		// AIController にアクセス
		TObjectPtr<AAIController> AIController = OwnerComp.GetAIOwner();
		if (AIController)
		{
			// ControlledPawn にアクセス
			APawn* ControlledPawn = AIController->GetPawn();
			if (ControlledPawn)
			{
				// 敵の位置
				double EnemyLocation = ControlledPawn->GetActorLocation().Size();
				
				// プレイヤーの位置
				TObjectPtr<AActor> TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKey.SelectedKeyName));
				double PlayerLocation = TargetActor->GetActorLocation().Size();

				// 敵とプレイヤーの距離が Distance より遠いか
				if (FMath::Abs(EnemyLocation - PlayerLocation) >= Distance)
				{
					return true;
				}
				else
				{
					return false;
				}
				
			}
		}
	}
	return false;
}

