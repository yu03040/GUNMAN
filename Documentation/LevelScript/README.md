# GUNMAN - マップ (LevelScript)

`ALevelScriptActor` を継承したレベルスクリプトクラスです。
各マップの UI 表示・Enhanced Input によるメニュー操作・ポーズ処理を担当します。

| ファイル | 対応シーン | 概要 |
|---|---|---|
| [BaseMapScript](BaseMapScript.md) | 全シーン共通 | 全 LevelScript の基底クラス。上下キーによるボタン選択・色変更の仮想関数を提供 |
| [TitleMapScript](TitleMapScript.md) | タイトル | タイトルメニューの UI 表示と入力バインド。ゲーム開始・終了・操作説明ボタンを管理 |
| [BattleMapScript](BattleMapScript.md) | バトル | ポーズメニューの表示/非表示・入力コンテキスト切り替えを管理 |
| [GameClearMapScript](GameClearMapScript.md) | ゲームクリア | クリア画面の UI 表示と「続ける / 終了」ボタンの入力処理 |
| [GameOverMapScript](GameOverMapScript.md) | ゲームオーバー | オーバー画面の UI 表示と「続ける / 終了」ボタンの入力処理 |

## 設計ポイント

`BaseMapScript` が上下移動・ボタン色変更の共通ロジックを持ち、各マップクラスが `ChangeButtonColor` と `UpdateOutputButton` をオーバーライドする形になっています。これにより、シーンごとのボタン数やアクションが異なっても共通の入力処理を再利用できます。
