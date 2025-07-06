# BTT_RunningToTarget クラスの概要

## 主な処理内容

`UBTT_RunningToTarget` クラスは、`UBTTask_BlackboardBase` を継承したタスクで、敵AIがプレイヤーに対してダッシュする行動を定義します。このタスクでは、敵AIがプレイヤーにダッシュし、プレイヤーに追いついた際にスピードを元に戻します。さらに、このタスクは5秒以内に追いつけない場合、自動的に中止され、スピードも元に戻ります。

- **継承クラス**: `UBTTask_BlackboardBase`
- **機能**: 敵AIがプレイヤーに向かってダッシュし、一定時間以内に追いつけない場合タスクを中止し、移動速度を元に戻します。

## このクラスのソースコードの説明

### コンストラクタ （`UBTT_RunningToTarget::UBTT_RunningToTarget`）
- **NodeName**: このタスクは `Behavior Tree` に「RunningToTarget」という名前で表示されます。エディタ内での識別が容易になります。

### `ExecuteTask` 関数
- **BlackboardComponent**: `Blackboard` に保存されているターゲットアクター（プレイヤー）の情報を取得します。
- **AIController**: 敵AIのコントローラーを取得します。
- **Enemy**: 敵AIのポーンを取得し、インターフェース `IAIEnemyInterface` を介して `ChangeRunningSpeed` 関数を実行します。
- **RunningSpeed の変更**: ターゲットに向かってダッシュする際に、敵の移動速度を一時的に `RunningSpeed` に変更します。
- **MoveToActor**: 敵AIがターゲットに向かって移動を開始し、5秒以内に追いつけた場合にタスクが成功します。追いついた際には `WalkSpeed` に戻されます。

### `AbortTask` 関数

#### 関数の動作概要
- タスクが中止された場合、敵AIの移動速度を `WalkSpeed` に戻します。
- **FinishLatentAbort**: タスクの中止が正常に完了したことを示します。

#### プロパティの説明
- **RunningSpeed**: 敵がプレイヤーにダッシュする際に設定されるスピード。
- **WalkSpeed**: 敵が通常の移動時に使用するスピード。追いついた後やタスクが中止された場合に適用されます。
- **TargetActorKey**: `Blackboard` 内のターゲットアクター（プレイヤー）のキーを指定します。