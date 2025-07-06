# BTD_FarFromTarget クラスの概要

## 主な処理内容

`UBTD_FarFromTarget` クラスは、`UBTDecorator_BlackboardBase` を継承して作成されたデコレータークラスです。このクラスは、敵AIがプレイヤーとの距離を確認するために使用されます。具体的には、プレイヤーと敵AIの位置を比較し、その距離が指定した `Distance` よりも遠いかどうかを調べる役割を持ちます。`Behavior Tree` の中で、この条件に基づいて次の行動を決定するために利用されます。

- **継承クラス**: `UBTDecorator_BlackboardBase`
- **機能**: 敵AIとプレイヤーの距離が指定された `Distance` より遠いかどうかを判定し、その結果を基にAIの行動を制御します。

## このクラスのソースコードの説明

### コンストラクタ （`UBTD_FarFromTarget::UBTD_FarFromTarget`）
- **NodeName**: このデコレーターは、`Behavior Tree` 内で「FarFromTarget」という名前で表示されます。これにより、デザイナーはエディタ上で識別しやすくなります。

### `CalculateRawConditionValue` 関数
- **BlackboardComponent**: `Behavior Tree` に関連する `Blackboard` のデータにアクセスします。
- **AIController**: 現在のAIのコントローラーにアクセスして、AIが操作するポーン（敵キャラクター）を取得します。
- **ControlledPawn**: AIが操作しているポーン（敵キャラクター）の位置情報を取得し、そのサイズを求めます。
- **TargetActor**: `Blackboard` からプレイヤーの情報を取得し、プレイヤーの位置を計算します。
- **距離判定**: 敵とプレイヤーの距離が指定された `Distance` よりも遠ければ `true` を返し、そうでなければ `false` を返します。この結果に基づいて、敵AIがプレイヤーから距離を保っているかどうかが判定され、次の行動に進みます。

#### プロパティの説明
- **TargetActorKey**: `Blackboard` にあるプレイヤーのアクター情報を指定します。これにより、プレイヤーの位置データにアクセス可能になります。
- **Distance**: 敵AIとプレイヤーの間の距離を計測する際に使用される閾値です。この距離を超えている場合に `true` が返されます。