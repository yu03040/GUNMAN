// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "EngineUtils.h" 
#include "BTT_TaskPath.generated.h"

/**
 * 
 */
UCLASS()
class GUNMAN_API UBTT_TaskPath : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

private:
	TArray<FVector> PathPoint;

	int PathIndex;

	UPROPERTY(EditAnywhere, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector TargetLocationKey;

	UPROPERTY(EditAnywhere, Category = "BlackBoard", meta = (AllowPrivateAccess = "true"))
	FBlackboardKeySelector TargetIndexKey;

protected:
	TSubclassOf<class APathPoint> something_object = nullptr;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UBTT_TaskPath();

	int SelectInt(bool bCondition, int OptionA, int OptionB);
};
