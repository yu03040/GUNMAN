# GUNMAN - 敵 AI

Behavior Tree と AI Perception を用いた敵キャラクターのシステムです。

## キャラクター・コントローラー

| ファイル | 概要 |
|---|---|
| [AIEnemy](AIEnemy.md) | 敵キャラクタークラス。体力管理・攻撃処理・ダメージ処理・Timeline による速度補間 |
| [AIEnemyController](AIEnemyController.md) | 敵の AI コントローラー。AI Perception（視覚）でプレイヤーを検知し Blackboard を更新、Behavior Tree を駆動 |
| [EnemyAnimInstance](EnemyAnimInstance.md) | 敵のアニメーションインスタンス。速度・方向・エイム角度（±50°）をフレームごとに更新 |

## スポーン・巡回ポイント

| ファイル | 概要 |
|---|---|
| [EnemyTargetPoint](EnemyTargetPoint.md) | 敵のスポーン地点アクター。ランダムな間隔で最大数まで敵を生成する |
| [PathPoint](PathPoint.md) | 敵の巡回ルートを定義するアクター。エディタのビジュアルウィジェットで座標を配置できる |

## インターフェース

| ファイル | 概要 |
|---|---|
| [AIEnemyInterface](AIEnemyInterface.md) | 敵キャラクターへの命令（攻撃・速度変更）を抽象化するインターフェース |
| [EnemyAIControllerInterface](EnemyAIControllerInterface.md) | AI コントローラーへの命令（Blackboard キー更新）を抽象化するインターフェース |

## Behavior Tree

Behavior Tree（`BT_PatrolAI`）で使用するカスタムタスク・デコレーターです。  
詳細は [`BehaviorTree/`](BehaviorTree/README.md) を参照してください。

| ファイル | 種別 | 概要 |
|---|---|---|
| [BTD_FarFromTarget](BehaviorTree/BTD_FarFromTarget.md) | Decorator | 敵とプレイヤーの距離を判定し、巡回と追跡のブランチを切り替える |
| [BTT_TaskPath](BehaviorTree/BTT_TaskPath.md) | Task | タグ（PathA / PathB / Random）に応じた巡回移動を実行する |
| [BTT_RunningToTarget](BehaviorTree/BTT_RunningToTarget.md) | Task | プレイヤーへダッシュ追跡。到達または中断時に速度をリセット |
| [BTT_ShootPlayer](BehaviorTree/BTT_ShootPlayer.md) | Task | `IAIEnemyInterface` 経由で敵に攻撃を実行させる |
