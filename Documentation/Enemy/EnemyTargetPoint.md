# EnemyTargetPoint クラスの概要

## 主な処理内容

`EnemyTargetPoint` クラスは、`ATargetPoint` を継承したクラスで、敵が発生するポイントを定義します。このクラスでは、敵の生成や出現間隔の設定を行い、複数の敵を一定間隔で発生させる機能を持っています。

- **Tickの設定**: コンストラクター内で `Tick` が毎フレーム呼び出されるように設定されています。
- **敵AIを生成する処理**: 指定された間隔で、指定された場所に敵AIを生成する処理を持っています。
- **最大生成数の制御**: 敵が一定数生成されたら、それ以上の生成を防ぐ機能も備えています。

## このクラスのソースコードの説明

### コンストラクター `（AEnemyTargetPoint::AEnemyTargetPoint）`
- **Tickの設定**:
  - `PrimaryActorTick.bCanEverTick = true;` によって、このアクターが毎フレーム `Tick()` を呼び出すように設定されています。これは、敵の生成を定期的に行うために必要です。
- **プロパティの初期化**:
  - 敵カウンター（`EnemyCounter`）、敵の最大数（`MaxEnemyCount`）、敵のブループリントクラス（`BPEnemyPawnClass`）、出現間隔（`SpawningIntervalTime`）などが初期化されています。

### `BeginPlay` 関数
- **ランダムな敵の出現間隔の設定**:
  - `UKismetMathLibrary::RandomFloatInRange` によって、`MinIntervalTime` から `MaxIntervalTime` の間でランダムな時間が計算され、出現間隔として使用されます。
- **敵の生成処理のタイマー設定**:
  - `FTimerManager` を用いて、`GenerateEnemyAI()` 関数がランダムな間隔で呼び出されるように設定されています。これにより、一定の時間ごとに敵が生成される仕組みが構築されています。

### `Tick` 関数
- 毎フレーム呼び出される関数で、特にカスタム処理は行われていませんが、将来的に更新処理を追加するためのものです。

### `GenerateEnemyAI` 関数
- **敵の生成**:
  - 敵のカウンター（`EnemyCounter`）が `MaxEnemyCount` に達していない場合に、敵AIキャラクターをスポーンします。`World->SpawnActor<APawn>` により指定された位置と回転で敵が生成されます。
- **AIコントローラーのアタッチ**:
  - 生成された敵に対して、`AIEnemyController` がアタッチされます。すでに存在している場合はそれを使用し、存在しない場合は新しいコントローラーを生成します。
  - `AIController` が適切に敵を操作できるように、`Possess()` 関数を用いて敵をコントロールするように設定されています。