// Fill out your copyright notice in the Description page of Project Settings.


#include "GUNMAN/Enemy/BehaviorTree/Tasks/BTT_TaskPath.h"
#include "GUNMAN/Enemy/AIEnemy.h"
#include "GUNMAN/Enemy/AIEnemyController.h"
#include "GUNMAN/Enemy/PathPoint.h"
#include "GameFramework/Actor.h"
#include "Engine/Blueprint.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"


UBTT_TaskPath::UBTT_TaskPath()
{
	// BehaviorTree に表示されるタスクノードの名前を設定
	NodeName = "TaskPath";

	// BP_PathPoint のロード
	static ConstructorHelpers::FObjectFinder<UClass> found_something_object(TEXT("/Game/GUNMAN/Blueprint/Enemy/BP_PathPoint.BP_PathPoint_C"));
	if (found_something_object.Succeeded())
	{
		something_object = found_something_object.Object;
	}
}

EBTNodeResult::Type UBTT_TaskPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TObjectPtr<UBlackboardComponent> BlackboardComponent = OwnerComp.GetBlackboardComponent();
	// AIController にアクセス
	AAIController* AIController = OwnerComp.GetAIOwner();
	// ControlledPawn にアクセス
	APawn* ControlledPawn = AIController->GetPawn();

	if (BlackboardComponent && AIController && ControlledPawn)
	{
		// 敵のタグが Random か？
		bool bIsTagRandom = ControlledPawn->ActorHasTag(FName(TEXT("Random")));
		if (bIsTagRandom)
		{
			FVector Origin = ControlledPawn->GetActorLocation();
			FNavLocation RandomLocation;
			TObjectPtr<UNavigationSystemV1> NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
			if (NavSys)
			{
				// 半径 10 m 以内にランダムに出現させる
				NavSys->GetRandomReachablePointInRadius(Origin, 1000.0f, RandomLocation, nullptr, nullptr);

				// ブラックボードに座標をセット
				BlackboardComponent->SetValueAsVector(TargetLocationKey.SelectedKeyName, RandomLocation.Location);

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
		else
		{
			// タグで巡回するルートを判断させる（タグには PathA, PathB のどちらかがくる）
			TArray<TObjectPtr<AActor>> FoundActors;
			UGameplayStatics::GetAllActorsOfClassWithTag(this, something_object, ControlledPawn->Tags[0], FoundActors);

			for (TObjectPtr<AActor>& FoundActor : FoundActors)
			{
				TObjectPtr<APathPoint> TargetActor = Cast<APathPoint>(FoundActor);
				if (TargetActor)
				{
					auto Enemy = Cast<AAIEnemy>(ControlledPawn);
					// ローカル座標からワールド座標に変換
					PathPoint = TargetActor->PathPoint;
					PathIndex = Enemy->index;
					FVector VectorValue = TargetActor->GetActorTransform().TransformPosition(PathPoint[PathIndex]);

					// ブラックボードに座標とインデックスをセット
					BlackboardComponent->SetValueAsVector(TargetLocationKey.SelectedKeyName, VectorValue);
					BlackboardComponent->SetValueAsInt(TargetIndexKey.SelectedKeyName, PathIndex);
					auto EnemyController = Cast<AAIEnemyController>(AIController);
					UE_LOG(LogTemp, Warning, TEXT("PathPoint[%d] = %s (%s, %s)"), PathIndex, *VectorValue.ToString(), *ControlledPawn->GetName(), *EnemyController->GetBTAsset()->GetName());

					// インデックスを超えたら 0 に戻す
					bool bCondition = (PathIndex + 1 < PathPoint.Num()) ? true : false;
					PathIndex = SelectInt(bCondition, PathIndex + 1, 0);
					Enemy->index = PathIndex;

					// タスクの成功
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}

	// タスクの失敗
	return EBTNodeResult::Failed;
}

int UBTT_TaskPath::SelectInt(bool bCondition, int OptionA, int OptionB)
{
	return bCondition ? OptionA : OptionB;
}