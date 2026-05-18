# EnemyTargetPoint クラスの概要

ソースコード: `Source/GUNMAN/Enemy/EnemyTargetPoint.h / .cpp`

## 概要

`AEnemyTargetPoint` は `ATargetPoint` を継承した敵スポーン地点アクターです。  
`BeginPlay` でランダムな間隔のタイマーを設定し、`MaxEnemyCount` に達するまで繰り返し敵を生成します。

このクラスを継承した Blueprint が 3 種類あり、それぞれ異なるパターンの敵を生成します。

| Blueprint | 生成する敵 |
|---|---|
| `BP_EnemyTargetPoint_Path_A` | `PathA` タグを持つ敵（巡回路 A） |
| `BP_EnemyTargetPoint_Path_B` | `PathB` タグを持つ敵（巡回路 B） |
| `BP_EnemyTargetPoint_Random` | `Random` タグを持つ敵（ランダム巡回） |

## プロパティ一覧

| プロパティ | 型 | 初期値 | 説明 |
|---|---|---|---|
| `EnemyCounter` | `int` | 0 | 現在の生成数カウンター |
| `MaxEnemyCount` | `int` | 0 | 生成する敵の最大数（エディタで設定） |
| `BPEnemyPawnClass` | `TSubclassOf<APawn>` | null | 生成する敵の Blueprint クラス（エディタで設定） |
| `SpawningIntervalTime` | `float` | 0 | 実際に使用されるスポーン間隔（`BeginPlay` でランダム決定） |
| `MinIntervalTime` | `float` | 0 | スポーン間隔の最小値（エディタで設定） |
| `MaxIntervalTime` | `float` | 0 | スポーン間隔の最大値（エディタで設定） |

## 関数の説明

### `AEnemyTargetPoint()` コンストラクタ

`PrimaryActorTick.bCanEverTick = true` を設定し、全プロパティを 0 / null で初期化します。

### `BeginPlay()`

1. `RandomFloatInRange(MinIntervalTime, MaxIntervalTime)` でスポーン間隔をランダム決定
2. `FTimerManager::SetTimer` で `GenerateEnemyAI` を `SpawningIntervalTime` 間隔で繰り返し呼び出すタイマーを設定

### `Tick(float DeltaTime)`

`Super::Tick` のみ。カスタム処理はありません。

### `GenerateEnemyAI(UWorld*, TSubclassOf<APawn>, const FVector&, const FRotator&)`

```mermaid
flowchart TD
    A{EnemyCounter < MaxEnemyCount ?}
    A -- No --> Z["何もしない（生成上限に達した）"]
    A -- Yes --> B["EnemyCounter++\nWorld->SpawnActor でポーンをスポーン"]
    B --> C{SpawnedPawn->GetController() が\nAAIEnemyController にキャストできるか？}
    C -- Yes --> E["既存コントローラーをそのまま使用"]
    C -- No --> D["BP_AIEnemyController をロードしてスポーン\nAIController->Possess(SpawnedPawn)"]
```

- `EnemyCounter++` を条件判定に使うため、生成上限ちょうどで停止します
- `SpawnCollisionHandlingOverride = AdjustIfPossibleButAlwaysSpawn` でコリジョンがあっても必ずスポーンします
