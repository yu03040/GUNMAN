# UIGunSight クラスの概要

ソースコード: `Source/GUNMAN/UMG/UIGunSight.h / .cpp`  
Blueprint: `WBP_UIGunSight`

## 概要

`UUIGunSight` は `UUserWidget` を継承したエイム用の照準（クロスヘア）ウィジェットです。  
C++ コードには関数・プロパティは一切なく、ウィジェットのレイアウトはすべて Blueprint 側で定義されています。

## 表示タイミング

`GUNMANCharacter::DisplayGunSight` で生成（`AddToViewport` はしない）し、  
以下のタイミングで表示・非表示を切り替えます。

| 関数 | 操作 |
|---|---|
| `StartReadyGun()` | `UIGunSightRef->AddToViewport()` — エイム開始時に表示 |
| `StopReadyGun()` | `UIGunSightRef->RemoveFromParent()` — エイム終了時に非表示 |
| `ToggleBetweenTPSAndFPS()` | TPS→FPS 遷移時に非表示、FPS→TPS かつエイム中なら再表示 |
