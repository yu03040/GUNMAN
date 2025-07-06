# TitleMapScript クラスの概要

## 主な処理内容

![Level_ClassDiagrams](Images/Level_ClassDiagrams.png)

`TitleMapScript` クラスは、`BaseMapScript` クラスを継承したタイトルマップ専用のクラスです。このクラスは、タイトル画面でのUI操作を管理する役割を持っています。

- **LevelScriptActorの継承**: このクラスは `LevelScriptActor` を継承し、レベル固有のロジックを実装しています。
- **EnhancedInputの利用**: `EnhancedInput` システムを使用して、タイトルマップ用の入力システムを構築しています。
- **UI管理**: タイトルメニューウィジェットを表示し、ユーザーがメニュー項目を選択できるようにします。
- **仮想関数のオーバーライド**: 親クラスから継承された仮想関数 (`ChangeButtonColor` や `UpdateOutputButton`) をオーバーライドし、選択されたボタンの色変更やクリックイベントを発生させます。

## 関数の説明

### コンストラクター（`ATitleMapScript::ATitleMapScript`）
- **UI関連のプロパティ初期化**: `UI_Title` を初期化し、`MaxButtonCounter` と `InvalidButtonIndex` を設定します。
- **EnhancedInputアセットのロード**: `ConstructorHelpers` を使用して、`EnhancedInput` の `InputMappingContext` と `InputAction` をロードします。これにより、プレイヤーが使用する入力アクションが設定されます。

### `BeginPlay` 関数
- **プレイヤーコントローラーの取得**: `UGameplayStatics::GetPlayerController` を使用して、現在のプレイヤーコントローラーを取得します。
- **マッピングコンテキストの追加**: `EnhancedInputLocalPlayerSubsystem` を通じて、ロードされた `InputMappingContext` をサブシステムに追加します。
- **入力セットアップ**: `SetupInput` 関数を呼び出し、プレイヤー入力コンポーネントにアクションをバインドします。
- **タイトルウィジェットの表示**: タイトルメニューのウィジェットを作成し、画面に表示します。最初に `Game Start` ボタンを選択状態に設定します。

### `SetupInput` 関数
- **入力アクションのバインド**: `EnhancedInputComponent` を使用して、`EnterAction`、`DownArrowKeyAction`、`UpArrowKeyAction` をそれぞれの関数 (`UpdateOutputButton`、`UI_DownwardMovement`、`UI_UpwardMovement`) にバインドします。

### `ChangeButtonColor` 関数
- **ボタン色の初期化**: `Game Start`、`Game End`、`Game Start Explanation` ボタンの色を白にリセットします。
- **選択状態のボタンの色を変更**: `ButtonCounter` の値に応じて、選択されたボタンの色を `SelectedColor` に変更します。

### `UpdateOutputButton` 関数
- **選択されたボタンのアクション実行**: `ButtonCounter` の値に基づき、対応するボタンがクリックされたときの動作を実行します。`Game Start Explanation` ボタンが選択された場合は、操作説明キャンバスを開き、そのキャンバスに対する終了ボタンの色を `SelectedColor` に変更します。