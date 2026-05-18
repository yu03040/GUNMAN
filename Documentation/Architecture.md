# GUNMAN - アーキテクチャ概要

## システム全体像

GUNMAN は以下の 6 つのモジュールで構成されています。

```mermaid
graph TD
    GM["GameMode / LevelScript\nシーン遷移・タイマー・敵スポーン・ポーズ管理"]

    PC["プレイヤーキャラクター\nAGUNMANCharacter\nEnhanced Input / デュアルカメラ / 武器システム"]
    AI["敵 AI\nAAIEnemy + AAIEnemyController\nBehavior Tree / AI Perception / 巡回・追跡・攻撃"]
    WP["武器システム\nAWeapon / FWeaponStructure / AWeaponAmmunition"]
    UI["UI (UMG)\nHUD・照準・タイマー・各種メニュー"]
    AN["アニメーション\nIAnimationInterface / GUNMANAnimInstance\nAnimNotify_AdmitFiring / AnimNotify_StopFiring"]

    GM --> PC
    GM --> AI
    AI -->|"ダメージ"| PC
    PC --> WP
    PC --> AN
    PC --> UI
```

---

## 設計方針

### Interface（インターフェース）駆動設計

システム間の依存を疎結合に保つため、4 つのインターフェースを使用しています。

| インターフェース | 実装クラス | 役割 |
|---|---|---|
| `IAnimationInterface` | `GUNMANAnimInstance` | アニメーション状態の制御（装備・エイム・発砲） |
| `IWeaponInterface` | `AGUNMANCharacter` | 武器のアタッチ通知受け取り |
| `IAIEnemyInterface` | `AAIEnemy` | 攻撃・移動速度変更の命令 |
| `IEnemyAIControllerInterface` | `AAIEnemyController` | Blackboard キーの更新 |

呼び出し側は具体的なクラスを知らなくてよいため、Blueprint 拡張や派生クラスへの差し替えが容易になっています。

### データ駆動武器システム

武器固有のパラメータは全て DataTable（`DT_Weapon`）に集約されており、C++ コード側には武器の種類ごとの分岐が存在しません。

```mermaid
graph TD
    DT["DT_Weapon (DataTable)"]
    FS["FWeaponStructure (1行 = 1武器)"]
    A1["AttachSocketName\nホルスター位置"]
    A2["EquipSocketName\n構え位置"]
    A3["EquipmentNoise\n装備 SE"]
    A4["HasPistol\nピストル判定フラグ"]
    A5["GunshotSound\n発射 SE"]
    A6["MuzzleFire\nマズルフラッシュ"]
    A7["FiringMontage\n発射アニメーション"]
    A8["AmmunitionClass\n薬莢クラス参照"]

    DT --> FS
    FS --> A1
    FS --> A2
    FS --> A3
    FS --> A4
    FS --> A5
    FS --> A6
    FS --> A7
    FS --> A8
```

武器を追加する場合は DataTable に行を追加するだけで対応できます。

### Behavior Tree による AI 設計

敵の行動は Behavior Tree（`BT_PatrolAI`）が管理し、C++ はタスク・デコレーターを提供します。

```mermaid
graph TD
    BT["BT_PatrolAI"]
    D["[Decorator] BTD_FarFromTarget\nプレイヤーとの距離が閾値を超えているか判定"]
    T1["[Task] BTT_TaskPath\nタグ (PathA / PathB / Random) に応じた巡回処理\n※ インデックスは敵個体が保持（共有しない）"]
    T2["[Task] BTT_RunningToTarget\nプレイヤーへ MoveToActor で追跡\n中断時に速度をリセット"]
    T3["[Task] BTT_ShootPlayer\nIAIEnemyInterface 経由で攻撃実行"]

    BT --> D
    BT --> T1
    BT --> T2
    BT --> T3
```

### Timeline による速度補間

プレイヤー・敵の移動速度変化は `FTimeline` + `FloatCurve` で制御しています。
`Tick` 内でタイムラインを更新し、`FMath::FInterpTo` で速度をスムーズに補間することで、
瞬時の速度変化によるアニメーションのぎこちなさを防いでいます。

### AnimNotify による発砲タイミング同期

アニメーションの進行とゲームロジックを同期させるため、AnimNotify を使用しています。

```mermaid
sequenceDiagram
    participant Anim as 発射アニメーション
    participant Admit as AnimNotify_AdmitFiring
    participant Stop as AnimNotify_StopFiring
    participant Char as GUNMANCharacter

    Anim->>Admit: 発砲許可フレーム到達
    Admit->>Char: FireState(true)
    Char->>Char: 発砲処理実行
    Anim->>Stop: 発砲停止フレーム到達
    Stop->>Char: FireState(false)
    Char->>Anim: 次のサイクルへ
```

これにより、連射速度はアニメーションの長さで自然に制御されます。

---

## シーン構成とクラスの対応

| シーン | GameMode | LevelScript | 主な UI |
|---|---|---|---|
| タイトル | `ATitleGameMode` | `ATitleMapScript` | `UITitle` |
| バトル | `AGUNMANGameMode` | `ABattleMapScript` | `UICharacter`, `UITimeLimitWidget`, `UIGunSight` |
| ゲームクリア | `AGameClearMode` | `AGameClearMapScript` | `UIGameClear` |
| ゲームオーバー | `AGameOverMode` | `AGameOverMapScript` | `UIGameOver` |

---

## クラス継承ツリー

### プレイヤー系

```mermaid
classDiagram
    ACharacter <|-- AGUNMANCharacter
    IAnimationInterface <|.. AGUNMANCharacter : implements
    IWeaponInterface <|.. AGUNMANCharacter : implements
    AGUNMANCharacter <|-- BP_ThirdPersonCharacter

    class AGUNMANCharacter {
        +OnFire()
        +ToggleBetweenTPSAndFPS()
        +AttachWeapon_Implementation()
    }
    class BP_ThirdPersonCharacter {
        <<Blueprint>>
    }
```

### 敵 AI 系

```mermaid
classDiagram
    ACharacter <|-- AAIEnemy
    IAIEnemyInterface <|.. AAIEnemy : implements
    AAIEnemy <|-- BP_EnemyAI_Path_A
    AAIEnemy <|-- BP_EnemyAI_Path_B
    AAIEnemy <|-- BP_EnemyAI_Path_Random

    AAIController <|-- AAIEnemyController
    IEnemyAIControllerInterface <|.. AAIEnemyController : implements

    class AAIEnemy {
        +AttackCharacter_Implementation()
        +ChangeRunningSpeed_Implementation()
    }
    class AAIEnemyController {
        +PerceptionUpdated()
        +UpdateTargetActorKey_Implementation()
    }
    class BP_EnemyAI_Path_A {
        <<Blueprint>>
    }
    class BP_EnemyAI_Path_B {
        <<Blueprint>>
    }
    class BP_EnemyAI_Path_Random {
        <<Blueprint>>
    }
```

### ゲームモード系

```mermaid
classDiagram
    AGameModeBase <|-- AGUNMANGameModeBase
    AGUNMANGameModeBase <|-- ATitleGameMode
    AGUNMANGameModeBase <|-- AGUNMANGameMode
    AGUNMANGameModeBase <|-- AGameClearMode
    AGUNMANGameModeBase <|-- AGameOverMode

    class AGUNMANGameMode {
        +Tick()
        +OpenGameClearMap()
        +OpenGameOverMap()
    }
```

### LevelScript 系

```mermaid
classDiagram
    ALevelScriptActor <|-- ABaseMapScript
    ABaseMapScript <|-- ATitleMapScript
    ABaseMapScript <|-- ABattleMapScript
    ABaseMapScript <|-- AGameClearMapScript
    ABaseMapScript <|-- AGameOverMapScript

    class ABaseMapScript {
        +UI_UpwardMovement()
        +UI_DownwardMovement()
        +ChangeButtonColor()*
        +UpdateOutputButton()*
    }
```

### 武器系

```mermaid
classDiagram
    AActor <|-- AWeapon
    AActor <|-- AWeaponAmmunition
    AActor <|-- AFirstPersonProjectile
    ATargetPoint <|-- AEnemyTargetPoint

    class AWeapon {
        +BeginPlay()
    }
    class AFirstPersonProjectile {
        +OnHit()
    }
    class AEnemyTargetPoint {
        +GenerateEnemyAI()
    }
```
