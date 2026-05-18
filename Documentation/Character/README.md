# GUNMAN - プレイヤーキャラクター

プレイヤーの操作・戦闘・カメラに関するクラスです。

## キャラクター

| ファイル | 概要 |
|---|---|
| [GUNMANCharacter](GUNMANCharacter.md) | プレイヤーキャラクターの中心クラス。移動・射撃・視点切り替え・武器管理・ダメージ処理をすべて担当 |
| [GUNMANController](GUNMANController.md) | プレイヤーコントローラー。現在は基底クラスの初期化のみ、将来拡張向けの土台 |

## アニメーション

アニメーション関連のドキュメントは [`Documentation/Animation/`](../Animation/README.md) を参照してください。

## 弾丸

| ファイル | 概要 |
|---|---|
| [FirstPersonProjectile](FirstPersonProjectile.md) | FPS モード専用の物理弾丸アクター。球体コリジョン（半径 12.5）と ProjectileMovement で飛翔・衝突処理 |
