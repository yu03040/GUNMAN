# GUNMANAnimInstance クラスの概要

## 主な処理内容

`GUNMANAnimInstance` クラスは、`UAnimInstance` を継承しており、キャラクターのアニメーション制御を行うクラスです。キャラクターが接地しているか、向いている方向、武器を構えているかの状態を管理します。また、`IAnimationInterface` を実装しており、他のクラス（例えば `GUNMANCharacter` クラス）からキャラクターのアニメーション状態を制御できるようになっています。

以下の2つの関数がオーバーライドされています。
- **AimingState_Implementation**: キャラクターが構えている状態を反映します。
- **EquippedState_Implementation**: キャラクターが武器やピストルを持っているかどうかの状態を反映します。

## このクラスのソースコードの説明

### コンストラクター（`UGUNMANAnimInstance::UGUNMANAnimInstance`）
コンストラクタです。特に初期化処理は行われていません。

### `NativeBeginPlay` 関数
  - アニメーションインスタンスの初期化時に呼ばれる関数で、親クラスの `NativeBeginPlay()` を呼び出した後、プレイヤーキャラクターの参照を取得します。`TryGetPawnOwner()` を使用して、アニメーションを適用するキャラクター (`PlayerCharacterRef`) を取得します。

### `NativeUpdateAnimation` 関数
毎フレームの更新処理を行う関数です。親クラスの `NativeUpdateAnimation()` を呼び出した後、以下の情報を更新します。
 - `bIsInAir`: キャラクターが空中にいるかどうかを `GetMovementComponent()->IsFalling()` を用いて判断します。
 - `Speed`: キャラクターの速度を取得し、1Dブレンドスペースで使用します。
 - `Direction`: キャラクターの移動方向を計算し、ステートマシンで使用します。
 - `AimPitch` と `AimYaw`: キャラクターの構え方向を計算し、ピッチ角度とヨー角度を `UKismetMathLibrary::ClampAngle()` で指定範囲に制限します。

### `AimingState_Implementation` 関数
キャラクターが武器を構えているかどうかを引数で受け取り、`IsAiming` フラグに格納します。これは他のクラスから呼び出され、アニメーションの状態を反映させます。

### `EquippedState_Implementation` 関数
キャラクターが武器を持っているか、ピストルを持っているかを受け取り、それぞれのフラグ (`HasWeapon`, `HasPistol`) に格納します。