# GUNMAN - GameMode

シーンごとのゲームルールを管理するクラスです。

| ファイル | 対応シーン | 概要 |
|---|---|---|
| [GUNMANGameModeBase](GUNMANGameModeBase.md) | 全シーン共通 | 全 GameMode の基底クラス。BeginPlay / Tick のフックを提供 |
| [TitleGameMode](TitleGameMode.md) | タイトル | タイトル画面用 GameMode。将来拡張向けの土台 |
| [GUNMANGameMode](GUNMANGameMode.md) | バトル | バトルマップ専用。制限時間管理・クリア/オーバー判定・シーン遷移 |
| [GameClearMode](GameClearMode.md) | ゲームクリア | クリア画面用 GameMode。将来拡張向けの土台 |
| [GameOverMode](GameOverMode.md) | ゲームオーバー | オーバー画面用 GameMode。将来拡張向けの土台 |

## 補足

- プレイヤーが死んだときのゲームオーバー遷移は `GUNMANCharacter` 側で処理しています
- 制限時間・クリア判定は `AGUNMANGameMode` の `Tick` で毎フレーム監視しています
