# アピールポイント

## 敵を AI によって使って動かす
このシューティングゲーム「GUNMAN」では、AIを使った敵キャラクターを実装しました。  
敵キャラクターは、 `ACharacter` クラスを基に作られた `AAIEnemy` クラスから派生した以下のブループリントクラス：  

 - 巡回路 A を回る敵 `BP_EnemyAI_Path_A`  
 - 巡回路 B を回る敵 `BP_EnemyAI_Path_B`  
 - ランダムに巡回する敵 `BP_EnemyAI_Path_Random`

で構成されています。  

さらに、敵が生成するポイントには `ATargetPoint` クラスを基に作られた `AEnemyTargetPoint` クラスから派生した以下のブループリントクラス：

- 巡回路 A を回る敵の発生地点 `BP_EnemyTargetPoint_Path_A`
- 巡回路 B を回る敵の発生地点 `BP_EnemyTargetPoint_Path_B`
- ランダムに巡回する敵の発生地点 `BP_EnemyTargetPoint_Random`

で構成されています。  

これらのブループリントクラスは、それぞれ異なるパターンの敵を生成させるための設定が含まれています。  これにより、プレイヤーは異なる動きをする敵を相手に戦うことができ、ゲームの戦略性が高まっています。

### 不具合や難しかったこと：

しかし、ゲームをプレイしてみると、 **敵が動かずその場に棒立ちになるという問題** に直面しました。ルートに配置された `BP_EnemyTargetPoint_Path_A` や `BP_EnemyTargetPoint_Path_B` 、または `BP_EnemyTargetPoint_Random` から敵が生成されるものの、期待していた巡回動作を行いませんでした。

### 原因と結果：

原因は、敵キャラクターである `AAIEnemy` の基底クラスにありました。 `AAIController` クラスを継承している `AAIEnemyController` クラスが敵を自動的に制御するタイミングを決定する設定が誤っていたためです。具体的には、敵キャラクターがAIにより制御される条件を設定する列挙型 `EAutoPossessAI` のオプションが間違っていました。元々、コードでは以下のように設定されていました。

```c++
AutoPossessAI = EAutoPossessAI::PlacedInWorld;
```

この設定では、レベル上に配置された敵にのみAIが適用されます。しかし、今回のゲームでは敵キャラクターが自動生成されるため、AIが適用されませんでした。正しい設定は次のように修正しました。

```C++
AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
```

この修正により、生成された敵キャラクターにもAIが適用されるようになり、指定のルートを巡回する動作が確認できました。

### 学び：

この経験を通して、AIキャラクターの制御タイミングについて深く学びました。特に、 `EAutoPossessAI` の設定が敵キャラクターの動作に大きく影響を与えることがわかりました。また、レベル上に配置された敵と自動生成される敵ではAIの設定が異なるため、どちらにも対応できるように柔軟な設定が必要であることを理解しました。これにより、今後のAIキャラクター開発においてより精度の高い制御ができるようになりました。

## 敵を正しい順序で巡回路を回らせる

このシューティングゲーム「GUNMAN」では、AIを用いて敵の巡回パターンを設計しました。  
敵キャラクターは、 `ACharacter` クラスを基に作られた `AAIEnemy` クラスから派生した以下のブループリントクラス：  

 - 巡回路 A を回る敵 `BP_EnemyAI_Path_A`  
 - 巡回路 B を回る敵 `BP_EnemyAI_Path_B`  
 - ランダムに巡回する敵 `BP_EnemyAI_Path_Random`

で構成されています。

敵はプレイヤーを発見していないときには指定されたルートを巡回するか、ランダムに動き回ります。この動作はビヘイビアツリーで管理され、巡回するかしないかの条件はデコレーターで、巡回ルートを回るやランダムに巡回するのはタスクで実現しています。  
特に、レベル上に配置した複数のパス（`BP_PathPoint` クラス）に沿って敵が巡回し、プレイヤーを発見されるまで継続する設計です。

これにより、AIの行動パターンにバリエーションが生まれ、プレイヤーはよりダイナミックな戦闘体験を楽しめるようになりました。

### 不具合や難しかったこと：

しかし、ゲームをテストした際に、 **巡回する敵がインデックスの順番通りに動かないという問題** が発生しました。具体的には、敵は指定されたルートを巡回するはずが、順番を飛ばして巡回してしまうケースがありました。例えば、 `0 → 1 → 2 → 3` の順で巡回してほしいところを、 `0 → 2 → 3 → 0` といったように順序がバラバラになることがあったため、期待通りの動作をしませんでした。

### 原因と結果：

この問題の原因は、タスク内で巡回インデックスを管理していたことにありました。

```C++
// ... 略

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
			// ... 略
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
					// PathIndex = Enemy->index; この行が必要!!!
					FVector VectorValue = TargetActor->GetActorTransform().TransformPosition(PathPoint[PathIndex]);

					// ブラックボードに座標とインデックスをセット
					BlackboardComponent->SetValueAsVector(TargetLocationKey.SelectedKeyName, VectorValue);
					BlackboardComponent->SetValueAsInt(TargetIndexKey.SelectedKeyName, PathIndex);
					auto EnemyController = Cast<AAIEnemyController>(AIController);
					UE_LOG(LogTemp, Warning, TEXT("PathPoint[%d] = %s (%s, %s)"), PathIndex, *VectorValue.ToString(), *ControlledPawn->GetName(), *EnemyController->GetBTAsset()->GetName());

					// インデックスを超えたら 0 に戻す
					bool bCondition = (PathIndex + 1 < PathPoint.Num()) ? true : false;
					PathIndex = SelectInt(bCondition, PathIndex + 1, 0);
					// Enemy->index = PathIndex; この行が必要!!!

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

// ... 略
```

`UBTT_TaskPath` クラスでは、敵ごとに巡回ルートを設定していましたが、敵が生成される際に同じタスクが複製されず、インデックスの管理が共有されてしまいました。そのため、複数の敵が同じ巡回ルート上でインデックスを上書きし合い、順序が乱れていました。この問題を解決するために、インデックスを敵側で管理するようにしました。各敵キャラクターが自分の巡回インデックスを持つことで、順序が乱れることなく正しく巡回するようになり、問題が解消されました。

### 学び：

この経験を通じて、AIキャラクターの動作を管理する際には、個々のキャラクターに適したデータ管理が必要であることを学びました。特に、タスク内で一括管理するのではなく、各キャラクターごとに必要な情報を持たせることが、複数の敵が正しく動作するために重要であると理解しました。これにより、AIプログラミングの効率的な方法や、データの管理方法についての知識を深めることができました。
