# UICharacter クラスの概要

ソースコード: `Source/GUNMAN/UMG/UICharacter.h / .cpp`  
Blueprint: `WBP_UICharacter`

## 概要

`UUICharacter` はプレイヤーの**体力バー**と**撃破数**を画面左上に表示する HUD ウィジェットです。  
`GUNMANCharacter::BeginPlay` で生成され、`AddToViewport` されます。

## ウィジェットコンポーネント

| コンポーネント | 型 | 役割 |
|---|---|---|
| `Health_ProgressBar` | `UProgressBar` | プレイヤーの体力割合を表示（`PercentDelegate` バインド） |
| `Kill_TextBlock` | `UTextBlock` | "KILL" などのラベルテキスト |
| `KillCount_TextBlock` | `UTextBlock` | 撃破数の数値を表示（`TextDelegate` バインド） |
| `MoveKillCount` | `UWidgetAnimation` | 撃破数テキストのアニメーション（`BindWidgetAnim`） |

## 関数の説明

### `Initialize()`

ウィジェット初期化時に呼ばれます。

- `KillCount_TextBlock->TextDelegate` に `SetKillCountText` をバインド
- `Health_ProgressBar->PercentDelegate` に `SetHealthProgressBar` をバインド

Delegate バインドにより、`SetKillCountText` と `SetHealthProgressBar` は毎フレーム自動で呼ばれます。

### `SetKillCountText()`

`GUNMANCharacter::GetKillCount()` で撃破数を取得し、`KillCount_TextBlock` に表示します。

アニメーション再生条件：

```
KillCount % 5 == 4 かつ KillCount != 0
```

| KillCount | 条件 | 動作 |
|---|---|---|
| 4 | `4 % 5 == 4` ✓ | `PlayAnimation(MoveKillCount)` |
| 5 | `5 % 5 == 0` ✗ | アニメーションなし |
| 9 | `9 % 5 == 4` ✓ | `PlayAnimation(MoveKillCount)` |
| 14 | `14 % 5 == 4` ✓ | `PlayAnimation(MoveKillCount)` |

つまり **4・9・14・19…** のタイミングで再生されます（5 の倍数になる直前のキル）。

### `SetHealthProgressBar()`

`GUNMANCharacter::CalcHealthPercent()`（= `CurrentHealth / MaxHealth`）の値を `Health_ProgressBar` に返します。  
プレイヤーが取得できない場合は `0.0f` を返します。
