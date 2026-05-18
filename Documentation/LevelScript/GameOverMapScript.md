# GameOverMapScript クラスの概要

ソースコード: `Source/GUNMAN/LevelScript/GameOverMapScript.h / .cpp`

## 概要

`AGameOverMapScript` は `ABaseMapScript` を継承した**ゲームオーバー画面専用の LevelScript** です。  
`AGameClearMapScript` と同一の構造で、対象ウィジェットのみが異なります。

クラス継承図は [BaseMapScript](BaseMapScript.md) を参照してください。

## プロパティ一覧

| プロパティ | 型 | 初期値 | 説明 |
|---|---|---|---|
| `UI_GameOver` | `UUIGameOver*` | null | ゲームオーバー画面ウィジェットの参照 |
| `DefaultMappingContext` | `UInputMappingContext*` | `IMC_OperatingUI` | UI 操作用の入力マッピング |
| `EnterAction` | `UInputAction*` | `IA_Enter` | 決定アクション |
| `DownArrowKeyAction` | `UInputAction*` | `IA_DownArrowKey` | 下移動アクション |
| `UpArrowKeyAction` | `UInputAction*` | `IA_UpArrowKey` | 上移動アクション |
| `MaxButtonCounter` | `int` | 2 | ボタン数 |
| `InvalidButtonIndex` | `int` | 3 | 範囲外インデックス |

## ボタンインデックスと対応

| ButtonCounter | ボタン | 処理 |
|---|---|---|
| 1 | Continue（タイトルへ） | `OnClickedContinue_Button()` → TitleMap へ遷移 |
| 2 | Game End | `OnClickedGameEnd_Button()` → アプリ終了 |

## 入力アクション一覧

| InputAction | トリガー | コールバック |
|---|---|---|
| `IA_Enter` | Triggered | `UpdateOutputButton` |
| `IA_DownArrowKey` | Triggered | `UI_DownwardMovement` |
| `IA_UpArrowKey` | Triggered | `UI_UpwardMovement` |

## 関数の説明

### `AGameOverMapScript()` コンストラクタ
- `MaxButtonCounter = 2`、`InvalidButtonIndex = 3`
- `IMC_OperatingUI` / `IA_Enter` / `IA_DownArrowKey` / `IA_UpArrowKey` をロード

### `BeginPlay()`
1. `IMC_OperatingUI` を `EnhancedInputLocalPlayerSubsystem` に追加
2. `SetupInput` で入力アクションをバインド
3. `WBP_GameOver` を同期ロードして `UUIGameOver` ウィジェットを生成・`AddToViewport`
4. `SetInputMode(FInputModeGameOnly)` でゲームパッド操作を有効化
5. `Continue` ボタンを `SelectedColor` で初期選択状態にする

### `SetupInput(TObjectPtr<UInputComponent>)`
`EnhancedInputComponent` に Enter・Down・Up アクションをバインドします。

### `ChangeButtonColor()`
`Continue` ボタンと `GameEnd` ボタンを白にリセットしてから、`ButtonCounter` に対応するボタンを `SelectedColor` にします。

### `UpdateOutputButton()`
`ButtonCounter == 1` なら `OnClickedContinue_Button()`（TitleMap 遷移）、  
`ButtonCounter == 2` なら `OnClickedGameEnd_Button()`（アプリ終了）を呼び出します。
