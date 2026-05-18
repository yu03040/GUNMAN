# UI_PauseMenu クラスの概要

ソースコード: `Source/GUNMAN/UMG/UI_PaseMenu.h / .cpp`  
Blueprint: `WBP_PaseMenu`

> **注意**: ソースコードのクラス名・ファイル名は `UI_PaseMenu`（"Pause" のスペルミス）です。

## 概要

`UUI_PaseMenu` はバトルマップのポーズメニューウィジェットです。  
`ABattleMapScript::InitializeButtonPosition` → `DisplayPauseMenu` で生成・表示されます。  
ボタンの背景色変更は `ABattleMapScript::ChangeButtonColor` が Getter 経由で直接操作します。

## ウィジェットコンポーネント

| コンポーネント | 型 | 役割 |
|---|---|---|
| `PoseMenu_TextBlock` | `UTextBlock` | メニュータイトルのラベル（※ソース上 "Pose" のスペルミス） |
| `BackToTitle_Button` | `UButton` | タイトルへ戻るボタン |
| `BackToTitle_TextBlock` | `UTextBlock` | タイトルへ戻るボタンのラベル |
| `Cancel_Button` | `UButton` | ゲームに戻るボタン |
| `Cancel_TextBlock` | `UTextBlock` | ゲームに戻るボタンのラベル |
| `GameEnd_Button` | `UButton` | ゲーム終了ボタン |
| `GameEnd_TextBlock` | `UTextBlock` | ゲーム終了ボタンのラベル |

## Getter 一覧

`ABattleMapScript::ChangeButtonColor` が選択状態のボタン色を変更するために使用します。

| 関数 | 戻り値 |
|---|---|
| `GetBackToTitle_Button()` | `UButton*` |
| `GetCancel_Button()` | `UButton*` |
| `GetGameEnd_Button()` | `UButton*` |

## 関数の説明

### `OnClickedBackToTitle_Button()`
`UGameplayStatics::OpenLevel(this, "TitleMap")` でタイトルマップへ遷移します。

### `OnClickedCancel_Button()`
1. `RemoveFromParent()` でポーズメニューをビューポートから削除
2. `UGameplayStatics::SetGamePaused(GetWorld(), false)` でポーズを解除してゲームを再開

### `OnClickedGameEnd_Button()`
`UKismetSystemLibrary::QuitGame(this, NULL, EQuitPreference::Quit, false)` でアプリを終了します。
