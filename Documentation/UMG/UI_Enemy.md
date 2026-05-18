# UIEnemy クラスの概要

ソースコード: `Source/GUNMAN/UMG/UIEnemy.h / .cpp`

## 概要

`UUIEnemy` は敵キャラクター頭上に表示する体力バーウィジェットです。  
`AAIEnemy` の `Widget`（`UWidgetComponent`）に設定され、`AAIEnemy::BeginPlay` で `SetOwningEnemy(this)` が呼ばれて参照先が確定します。

## ウィジェットコンポーネント

| コンポーネント | 型 | 役割 |
|---|---|---|
| `Health_ProgressBar` | `UProgressBar` | 敵の体力割合を表示（`PercentDelegate` バインド） |
| `OwningEnemy` | `AAIEnemy*` | 体力を参照する対象の敵（`SetOwningEnemy` で設定） |

## 関数の説明

### `Initialize()`

`Health_ProgressBar->PercentDelegate` に `SetHealthProgressBar` をバインドします。  
Delegate バインドにより毎フレーム自動で体力割合が更新されます。

### `SetHealthProgressBar()`

`OwningEnemy->GetHealthPercent()` の値を返します。  
`OwningEnemy` が未設定の場合は `0.0f` を返します。

### `SetOwningEnemy(TObjectPtr<AAIEnemy> NewOwner)`

`OwningEnemy` に `NewOwner` を設定します。  
`AAIEnemy::BeginPlay` から呼ばれます。
