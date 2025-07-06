# AIEnemyController クラスの概要

## 主な処理内容

`AIEnemyController` クラスは、`AAIController` を継承した AI キャラクターである `AIEnemy` を制御するためのコントローラーです。このクラスでは、AI の視覚に関連する機能を提供し、ビヘイビアツリー（Behavior Tree）を通じて敵の意思決定を管理します。コンストラクターでは、AI の知覚コンポーネントの設定、視覚情報の定義、ビヘイビアツリーのロードを行います。また、AI がプレイヤーを感知した際の処理や、プレイヤーを見失った際の対処も実装されています。

## このクラスのソースコードの説明

### コンストラクター （`AAIEnemyController::AAIEnemyController`）
- **AI Perception の作成**: AI が周囲の情報を感知するための `UAIPerceptionComponent` を作成し、視覚を通じて情報を取得できるように設定しています。
- **視覚の設定**: `UAISenseConfig_Sight` を使って AI の視覚範囲、周辺視野角度、対象の検知範囲（敵、ニュートラル、味方）を定義しています。
- **PerceptionUpdated イベントのバインド**: `OnTargetPerceptionUpdated` イベントに `PerceptionUpdated` 関数を関連付けて、視覚情報が更新された際の処理を定義しています。
- **ビヘイビアツリーのロード**: `ConstructorHelpers::FObjectFinder<UBehaviorTree>` を使用してビヘイビアツリー（`BT_PatrolAI`）をロードし、AI の行動を管理するための準備をしています。

### `BeginPlay` 関数
- **ビヘイビアツリーの開始**: ゲーム開始時にビヘイビアツリーを実行し、AI の行動を管理します。

このクラスは、AI の感知機能とビヘイビアツリーを組み合わせることで、動的な意思決定を行うためのフレームワークを提供します。### `UpdateTargetActorKey_Implementation` 関数
- **Blackboard の更新**: この関数は、AI が現在追跡している対象のアクターを `BlackboardComponent` にセットします。これにより、AI がターゲットを認識して行動を変更できます。

### `UpdateHasLineOfSightKey_Implementation` 関数
- **視界の有無を BlackBoard にセット**: AI がターゲットを視界に捉えているかどうかを `BlackboardComponent` に設定する関数です。これにより、ターゲットが視界にあるかどうかを他のシステムで参照できます。

### `PerceptionUpdated` 関数
- **視覚の更新処理**: AI が新たに感知したアクター（`Actor`）がプレイヤーかどうかを判定し、プレイヤーを認識した場合は `BlackboardComponent` を通じてターゲット情報を更新します。また、視界からプレイヤーが外れた場合は、一定時間後にターゲットをリセットします。

### `TargetLost` 関数
- **ターゲットを見失ったときの処理**: AI が一定時間プレイヤーを視界に捉えられなかった場合、ターゲットの情報を `BlackboardComponent` からクリアします。