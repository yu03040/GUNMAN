## (1) BTT_ShootPlayer クラスの概要

`BTT_ShootPlayer` クラスは、`UBTTask_BlackboardBase` を継承したタスクで、敵AIがプレイヤーを攻撃する行動を実装しています。このタスクは、敵AIがプレイヤーを検知した際に実行され、`Behavior Tree` 内でプレイヤーに向かって射撃などの攻撃を行うことを目的としています。

- **継承クラス**: `UBTTask_BlackboardBase`
- **機能**: 敵AIがプレイヤーを検知し、攻撃を実行します。

## (2) このクラスのソースコードの説明

### コンストラクタ（`UBTTask_BlackboardBase::UBTTask_BlackboardBase`）
- **NodeName**: このタスクは `Behavior Tree` 内で「ShootPlayer」として表示されます。エディタで視覚的にタスクを認識しやすくするための名前設定です。

### `ExecuteTask` 関数
- **AIController**: この関数は、敵AIのコントローラーにアクセスします。`AIController` が正しく取得できた場合、続いて敵AIのポーン（キャラクター）にアクセスします。
- **Enemy**: 取得したポーンを `AAIEnemy` 型にキャストして敵AIとして扱います。
- **Interface**: 敵AIが `IAIEnemyInterface` を実装している場合、`Execute_AttackCharacter` 関数を呼び出し、プレイヤーへの攻撃を実行します。
- **FinishLatentTask**: 攻撃が成功した場合、`Behavior Tree` にタスクの完了を報告し、タスクを `Succeeded` として終了します。