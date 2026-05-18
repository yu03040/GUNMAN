# EnemyAnimInstance クラスの概要

ソースコード: `Source/GUNMAN/Enemy/EnemyAnimInstance.h / .cpp`

## 概要

`UEnemyAnimInstance` は `UAnimInstance` を継承した敵専用のアニメーションインスタンスクラスです。  
`GUNMANAnimInstance`（プレイヤー用）と同じ更新ロジックを持ちますが、`IAnimationInterface` は実装せず、エイム角のクランプ範囲が異なります。

## プロパティ一覧

| プロパティ | 型 | 初期値 | 説明 |
|---|---|---|---|
| `EnemyCharacterRef` | `ACharacter*` | — | アニメーション対象キャラクターへの参照 |
| `bIsInAir` | `bool` | false | 空中にいるか（ステートマシンで使用） |
| `Speed` | `float` | 0.0 | 移動速度（1D ブレンドスペースで使用） |
| `Direction` | `float` | 0.0 | 移動方向 |
| `AimPitch` | `float` | 0.0 | 構え向き・ピッチ角（**-50〜50°** にクランプ） |
| `AimYaw` | `float` | 0.0 | 構え向き・ヨー角（**-50〜50°** にクランプ） |

> **プレイヤーとの違い**: `GUNMANAnimInstance` のクランプ範囲は ±90° ですが、  
> 敵は ±50° に制限されています。

## 関数の説明

### `NativeBeginPlay()`
`TryGetPawnOwner()` でオーナーキャラクターを取得し `EnemyCharacterRef` に代入します。

### `NativeUpdateAnimation(float DeltaTime)`

毎フレーム呼ばれ、以下のプロパティを更新します：

| プロパティ | 更新方法 |
|---|---|
| `bIsInAir` | `GetMovementComponent()->IsFalling()` |
| `Speed` | `GetVelocity().Size()` |
| `Direction` | `UKismetAnimationLibrary::CalculateDirection(Velocity, BaseRotation)` |
| `AimPitch` / `AimYaw` | コントロール回転とアクター回転の差分を `RInterpTo`（補間速度 5.0）で補間し **-50〜50°** にクランプ |
