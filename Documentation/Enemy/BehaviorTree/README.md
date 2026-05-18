# GUNMAN - Behavior Tree

ソースコードの対応場所:  
- `Source/GUNMAN/Enemy/BehaviorTree/Decorators/`  
- `Source/GUNMAN/Enemy/BehaviorTree/Tasks/`

敵 AI の行動を制御する Behavior Tree（`BT_PatrolAI`）で使用するカスタムタスク・デコレーターです。

## BT_PatrolAI の構成

```mermaid
flowchart TD
    BT["BT_PatrolAI（ルート）"]

    BT --> SEQ_PATROL["Selector"]

    SEQ_PATROL --> SEQ_CHASE["Sequence：追跡"]
    SEQ_PATROL --> SEQ_WALK["Sequence：巡回"]

    SEQ_CHASE --> DEC["[Decorator]\nBTD_FarFromTarget\n敵とプレイヤーの距離が\nDistance 未満なら実行"]
    SEQ_CHASE --> T2["[Task]\nBTT_RunningToTarget\nプレイヤーへダッシュ追跡"]
    SEQ_CHASE --> T3["[Task]\nBTT_ShootPlayer\n攻撃実行"]

    SEQ_WALK --> T1["[Task]\nBTT_TaskPath\nタグに応じた巡回移動"]
```

## ファイル一覧

| ファイル | 種別 | 概要 |
|---|---|---|
| [BTD_FarFromTarget](BTD_FarFromTarget.md) | Decorator | 敵とプレイヤーの距離を判定し、巡回と追跡のブランチを切り替える |
| [BTT_TaskPath](BTT_TaskPath.md) | Task | タグ（PathA / PathB / Random）に応じた巡回移動を実行する |
| [BTT_RunningToTarget](BTT_RunningToTarget.md) | Task | プレイヤーへダッシュ追跡。到達または中断時に速度をリセット |
| [BTT_ShootPlayer](BTT_ShootPlayer.md) | Task | `IAIEnemyInterface` 経由で敵に攻撃を実行させる |
