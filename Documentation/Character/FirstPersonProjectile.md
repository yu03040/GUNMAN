# FirstPersonProjectile クラスの概要

ソースコード: `Source/GUNMAN/SpawnActors/FirstPersonProjectile.h / .cpp`

## 概要

`AFirstPersonProjectile` は `AActor` を継承した、FPS モード専用の弾丸（プロジェクタイル）アクターです。  
`BP_FirstPersonProjectile` はこのクラスを継承した Blueprint クラスです。

- 球体コリジョン（`USphereComponent`）で衝突判定
- `UProjectileMovementComponent` で飛翔・バウンドを制御
- 衝突時に物理オブジェクトへの衝撃を与えて自爆
- 生成から 3 秒後に自動消滅

## コンポーネント一覧

| コンポーネント | 型 | 説明 |
|---|---|---|
| `CollisionComp` | `USphereComponent` | 衝突判定用の球体（半径 12.5）。ルートコンポーネント |
| `ProjectileMovement` | `UProjectileMovementComponent` | 飛翔制御。初速・最大速度 3000、バウンドあり |

## 関数の説明

### `AFirstPersonProjectile()` コンストラクタ

| 設定 | 値 |
|---|---|
| 球体コリジョン半径 | 12.5f |
| コリジョンプロファイル | `"Projectile"` |
| 初速度 / 最大速度 | 3000.0f |
| `bRotationFollowsVelocity` | true（飛行方向に回転） |
| `bShouldBounce` | true（バウンドあり） |
| `InitialLifeSpan` | 3.0f（3 秒後に自動消滅） |

### `OnHit()`

弾丸が何かに衝突したときに `OnComponentHit` から呼ばれます。

1. 衝突相手が物理シミュレーション中（`IsSimulatingPhysics()`）の場合、`GetVelocity() * 100.0f` のインパルスを与える
2. `Destroy()` で弾丸を消滅させる
