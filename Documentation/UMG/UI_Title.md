# UITitle クラスの概要

ソースコード: `Source/GUNMAN/UMG/UITitle.h / .cpp`  
Blueprint: `WBP_Title`

## 概要

`UUITitle` はタイトル画面のメニューウィジェットです。  
`ATitleMapScript::BeginPlay` で生成・表示され、ボタンの背景色変更は `ATitleMapScript` が Getter 経由で直接操作します。

## ウィジェットコンポーネント

| コンポーネント | 型 | 役割 |
|---|---|---|
| `GameStart_Button` | `UButton` | ゲーム開始ボタン |
| `GameStart_TextBlock` | `UTextBlock` | ゲーム開始ボタンのラベル |
| `GameEnd_Button` | `UButton` | ゲーム終了ボタン |
| `GameEnd_TextBlock` | `UTextBlock` | ゲーム終了ボタンのラベル |
| `GameStartExplaination_Button` | `UButton` | 操作説明を開くボタン（?ボタン） |
| `GameEndExplaination_Button` | `UButton` | 操作説明を閉じるボタン |
| `Explaination_CanvasPanel` | `UCanvasPanel` | 操作説明画像を含むパネル |

## Getter 一覧

`ATitleMapScript::ChangeButtonColor` が選択状態のボタン色を変更するために使用します。

| 関数 | 戻り値 | 用途 |
|---|---|---|
| `GetGameStart_Button()` | `UButton*` | GameStart ボタンの背景色変更 |
| `GetGameEnd_Button()` | `UButton*` | GameEnd ボタンの背景色変更 |
| `GetGameStartExplaination_Button()` | `UButton*` | ?ボタンの背景色変更 |
| `GetGameEndExplaination_Button()` | `UButton*` | 操作説明を閉じるボタンの背景色変更 |

## 関数の説明

### `OnClickedGameStart_Button()`
`UGameplayStatics::OpenLevel(this, "BattleMap")` でバトルマップへ遷移します。

### `OnClickedGameEnd_Button()`
`UKismetSystemLibrary::QuitGame(this, NULL, EQuitPreference::Quit, false)` でアプリを終了します。

### `OnClickedGameStartExplaination_Button()`
`Explaination_CanvasPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible)` で操作説明パネルを表示します。  
`SelfHitTestInvisible` は見た目は表示しつつ、このパネル自体はヒットテストを受け付けません。

### `OnClickedGameEndExplaination_Button()`
`Explaination_CanvasPanel->SetVisibility(ESlateVisibility::Hidden)` で操作説明パネルを非表示にします。
