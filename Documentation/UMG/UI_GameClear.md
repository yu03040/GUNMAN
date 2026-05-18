# UIGameClear クラスの概要

ソースコード: `Source/GUNMAN/UMG/UIGameClear.h / .cpp`  
Blueprint: `WBP_GameClear`

## 概要

`UUIGameClear` はゲームクリア画面のウィジェットです。  
`AGameClearMapScript::BeginPlay` で生成・表示されます。  
ボタンの背景色変更は `AGameClearMapScript::ChangeButtonColor` が Getter 経由で直接操作します。

## ウィジェットコンポーネント

| コンポーネント | 型 | 役割 |
|---|---|---|
| `Continue_Button` | `UButton` | タイトルへ戻るボタン |
| `Continue_TextBlock` | `UTextBlock` | Continue ボタンのラベル |
| `GameEnd_Button` | `UButton` | ゲーム終了ボタン |
| `GameEnd_TextBlock` | `UTextBlock` | ゲーム終了ボタンのラベル |

## Getter 一覧

`AGameClearMapScript::ChangeButtonColor` が選択状態のボタン色を変更するために使用します。

| 関数 | 戻り値 |
|---|---|
| `GetContinue_Button()` | `UButton*` |
| `GetGameEnd_Button()` | `UButton*` |

## 関数の説明

### `OnClickedContinue_Button()`
`UGameplayStatics::OpenLevel(this, "TitleMap")` でタイトルマップへ遷移します。

### `OnClickedGameEnd_Button()`
`UKismetSystemLibrary::QuitGame(this, NULL, EQuitPreference::Quit, false)` でアプリを終了します。
