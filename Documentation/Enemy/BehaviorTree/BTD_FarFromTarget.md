# BTD_FarFromTarget クラスの概要

ソースコード: `Source/GUNMAN/Enemy/BehaviorTree/Decorators/BTD_FarFromTarget.h / .cpp`

## 概要

`UBTD_FarFromTarget` は `UBTDecorator_BlackboardBase` を継承したデコレーターです。  
Behavior Tree エディタ上の表示名は **"FarFromTarget"** です。

`CalculateRawConditionValue` は距離差が `Distance` **以上**のとき `true` を返します。  
Behavior Tree エディタ側の設定（Inverse フラグなど）と組み合わせて、追跡・攻撃ブランチの実行条件を制御します。

## プロパティ

| プロパティ | 型 | 説明 |
|---|---|---|
| `TargetActorKey` | `FBlackboardKeySelector` | Blackboard 上のプレイヤーアクターキー |
| `Distance` | `float` | 追跡に切り替える距離の閾値（エディタで設定） |

## 関数の説明

### `CalculateRawConditionValue(UBehaviorTreeComponent&, uint8*) const`

```mermaid
flowchart TD
    A["BlackboardComponent から TargetActor を取得"]
    A --> B["EnemyLocation = ControlledPawn->GetActorLocation().Size()"]
    B --> C["PlayerLocation = TargetActor->GetActorLocation().Size()"]
    C --> D{FMath::Abs(EnemyLocation - PlayerLocation) >= Distance ?}
    D -- true  --> E["return true（条件成立：距離が閾値以上）"]
    D -- false --> F["return false（条件不成立：距離が閾値未満）"]
```

> **実装の注意点**  
> 距離の計算は `GetActorLocation().Size()`（原点からの距離）の差の絶対値を使っています。  
> 2 つのアクター間の直接距離（`FVector::Dist`）とは異なる値になる場合があります。

1. Blackboard から `TargetActor` を取得
2. 敵の位置ベクトルの大きさ（原点からの距離）と、プレイヤーの位置ベクトルの大きさの差の絶対値を計算
3. その値が `Distance` 以上なら `true` を返す
