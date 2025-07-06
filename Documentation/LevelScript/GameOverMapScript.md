# GameOverMapScript クラスの概要

## 主な処理内容

![Level_ClassDiagrams](Images/Level_ClassDiagrams.png)

`AGameOverMapScript` は、`LevelScriptActor` および `BaseMapScript` を継承したゲームオーバーマップ専用のクラスです。このクラスでは、ゲームオーバー画面の管理と操作が行われ、以下の機能を提供します。

- **プロパティのデフォルト設定**: コンストラクターで `MaxButtonCounter` や `InvalidButtonIndex` などのプロパティが設定される。
- **Enhanced Input** のロード: `InputAction` と `InputMappingContext` を使用してゲームオーバーマップ用の入力システムを構築。
- **ウィジェットの表示**: `UIGameOver` ウィジェットが表示され、ユーザーにゲームオーバーの選択肢を提供。
- **仮想関数のオーバーライド**:
  - **ボタン色の変更**: 選択されたボタンの色を変更。
  - **クリックイベントの処理**: ボタンが選択された際にイベントを発生させる。

## 関数の説明

### コンストラクター（`AGameOverMapScript::AGameOverMapScript`）
- `UI_GameOver`, `MaxButtonCounter`, `InvalidButtonIndex` などのデフォルトプロパティが設定される。
- `Enhanced Input` のアセット (`InputMappingContext`, `InputAction`) を `ConstructorHelpers::FObjectFinder` を使用してロード。

### `BeginPlay` 関数
- 親クラスの `BeginPlay` を呼び出した後、以下の初期化処理が行われる:
  1. **プレイヤーコントローラーの取得**: `UGameplayStatics::GetPlayerController` を使ってプレイヤーのコントローラーを取得。
  2. **マッピングコンテキストの追加**: `UEnhancedInputLocalPlayerSubsystem` を介して `DefaultMappingContext` をサブシステムに追加。
  3. **ウィジェットのセットアップ**: `WidgetClass` をブループリントから生成し、ゲームオーバーウィジェット (`UIGameOver`) を作成してビューポートに追加。
  4. **入力モードの設定**: プレイヤーの入力モードをゲームパッド操作用に設定。
  5. **初期ボタンの選択状態**: `Continue` ボタンを選択状態に設定し、背景色を `SelectedColor` に変更。

### `SetupInput` 関数
- **入力バインディングの設定**:
  - `EnhancedInputComponent->BindAction` によって、`EnterAction`, `DownArrowKeyAction`, `UpArrowKeyAction` がそれぞれの処理 (`UpdateOutputButton`, `UI_DownwardMovement`, `UI_UpwardMovement`) にバインドされる。

### `ChangeButtonColor` 関数
- ボタンの選択状態に基づいて、背景色を変更する処理を行う。`ButtonCounter` の値に応じて、`ContinueButton` または `EndButton` の色が `SelectedColor` に変わる。

### `UpdateOutputButton` 関数
- 選択されたボタンに応じて、`UI_GameOver->OnClickedContinue_Button` または `UI_GameOver->OnClickedGameEnd_Button` のクリックイベントを発生させる。