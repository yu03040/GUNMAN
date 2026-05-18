# GUNMAN - ソースコードドキュメント

## ゲーム概要

**GUNMAN** は Unreal Engine 5.3.2 で開発されたサバイバルアクション FPS/TPS ゲームです。

- **目標**: 制限時間 2 分以内に 10 体の敵を全て倒す
- **視点切り替え**: TPS（三人称）/ FPS（一人称）をリアルタイムに切り替え可能
- **敗北条件**: 体力がゼロになる、または制限時間切れ

### 武器一覧

| 武器 | 速度 | 威力 | 特徴 |
|------|------|------|------|
| ピストル | 中 | 中 | バランス型 |
| ショットガン | 低 | 高 | 近距離に強い |
| ライフル | 高 | 低 | 連射向き |

## 開発環境

| 項目 | 内容 |
|------|------|
| エンジン | Unreal Engine 5.3.2 |
| 言語 | C++ |
| 入力システム | Enhanced Input System |
| AI | Behavior Tree + AI Perception |
| UI | UMG (Unreal Motion Graphics) |
| ターゲット | Windows 10/11 |

## ゲームフロー

```
タイトル画面（武器選択）
    ↓ ゲーム開始
バトルマップ（2分タイマー）
    ├── 敵を10体撃破  ──→ ゲームクリア画面
    └── 体力0 / 時間切れ ──→ ゲームオーバー画面
```

## ドキュメント一覧

| ドキュメント | 内容 |
|---|---|
| [アーキテクチャ概要](Architecture.md) | システム全体の設計・モジュール間の関係 |
| [特に頑張ったところ](AppealPoint.md) | 実装上の工夫・バグ修正の記録 |
| [プレイヤーキャラクター](Character/README.md) | 移動・射撃・視点切り替え・武器管理 |
| [アニメーション](Animation/README.md) | AnimInstance・AnimNotify・アニメーションインターフェース |
| [敵 AI](Enemy/README.md) | Behavior Tree による巡回・追跡・攻撃 |
| [ゲームモード](GameMode/README.md) | シーンごとのゲームルール管理 |
| [マップ (LevelScript)](LevelScript/README.md) | 各マップの UI・入力制御 |
| [UI (UMG)](UMG/README.md) | HUD・メニュー・ウィジェット |
| [武器](Weapon/README.md) | 武器データ・アタッチ処理・弾薬 |
