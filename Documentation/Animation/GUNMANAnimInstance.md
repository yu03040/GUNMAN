# GUNMANAnimInstance クラスの概要

ソースコード: `Source/GUNMAN/Animations/GUNMANAnimInstance.h / .cpp`

## 概要

`UGUNMANAnimInstance` は `UAnimInstance` を継承し、`IAnimationInterface` を実装するプレイヤー TPS 用のアニメーションインスタンスクラスです。  
毎フレーム `NativeUpdateAnimation` でキャラクターの状態を取得し、アニメーションブループリントが参照するプロパティを更新します。

## プロパティ一覧

| プロパティ | 型 | 初期値 | 説明 |
|---|---|---|---|
| `IsAiming` | `bool` | false | エイム中か（`AimingState_Implementation` が更新） |
| `HasWeapon` | `bool` | false | 武器を装備中か（`EquippedState_Implementation` が更新） |
| `HasPistol` | `bool` | false | ピストル系か（`EquippedState_Implementation` が更新） |
| `PlayerCharacterRef` | `ACharacter*` | — | アニメーション対象キャラクターへの参照 |
| `bIsInAir` | `bool` | false | 空中にいるか（ステートマシンで使用） |
| `Speed` | `float` | 0.0 | 移動速度（1D ブレンドスペースで使用） |
| `Direction` | `float` | 0.0 | 移動方向（ステートマシンで使用） |
| `AimPitch` | `float` | 0.0 | 構え向き・ピッチ角（-90〜90°にクランプ） |
| `AimYaw` | `float` | 0.0 | 構え向き・ヨー角（-90〜90°にクランプ） |

## 関数の説明

### `NativeBeginPlay()`
`TryGetPawnOwner()` でオーナーキャラクターを取得し、`PlayerCharacterRef` に代入します。

### `NativeUpdateAnimation(float DeltaTime)`
毎フレーム呼ばれ、以下のプロパティを更新します：

| プロパティ | 更新方法 |
|---|---|
| `bIsInAir` | `GetMovementComponent()->IsFalling()` |
| `Speed` | `GetVelocity().Size()` |
| `Direction` | `UKismetAnimationLibrary::CalculateDirection(Velocity, BaseRotation)` |
| `AimPitch` / `AimYaw` | コントロール回転とアクター回転の差分を `RInterpTo`（補間速度 5.0）で補間し `-90〜90°` にクランプ |

### `AimingState_Implementation(bool bIsAiming)`
`IAnimationInterface::AimingState` の実装。引数をそのまま `IsAiming` に格納します。

### `EquippedState_Implementation(bool bHasWeapon, bool bHasPistol)`
`IAnimationInterface::EquippedState` の実装。引数をそれぞれ `HasWeapon` / `HasPistol` に格納します。
