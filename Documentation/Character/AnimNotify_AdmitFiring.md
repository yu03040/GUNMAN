# AnimNotify_AdmitFiring クラスの概要

## 主な処理内容

`UAnimNotify_AdmitFiring` クラスは、Unreal Engine の `UAnimNotify` を継承したクラスで、アニメーション再生中の特定のフレームで発砲を許可する通知を担当しています。このクラスは以下の特徴を持ちます:

- **発砲アニメーションの特定フレームでイベントをトリガー**:
  - ハンドガンやショットガンのアニメーションの指定されたタイミングで呼び出され、発砲の連射を許可します。
- **発砲許可の制御**:
  - `GUNMANCharacter` クラスが実装する `IAnimationInterface` の `FireState` 関数を呼び出し、発砲を許可するために `true` を渡します。
- **`Notify` 関数のオーバーライド**:
  - アニメーション中に特定のタイミングでこの関数が呼び出され、キャラクターの発砲状態を変更します。

## 関数の説明

### `Notify` 関数
- この `Notify` 関数は、アニメーション再生中にトリガーされる通知コールバックです。
- `MeshComp` (アニメーションを再生している `USkeletalMeshComponent`) を通じて、キャラクターの発砲状態を操作します。
- `MeshComp` のオーナーである `AGUNMANCharacter` を取得し、そのキャラクターが `IAnimationInterface` を実装している場合、`FireState` 関数を呼び出します。
- `FireState` 関数に `true` を渡すことで、発砲が許可されます。

- **Notify メカニズム**:  
この `AnimNotify` クラスは、アニメーションとゲームプレイのロジックを連動させる仕組みです。特定のフレームで `Notify` 関数が呼び出されることで、ゲーム内での発砲許可を制御し、リアルタイムでの連射管理を実現します。

### `GetNotifyName_Implementation` 関数
- この関数は、通知名を表示するためのオーバーライド関数です。
- `"AdmitFiring"` という文字列を返し、この通知が発砲を許可する役割を持つことを明示しています。