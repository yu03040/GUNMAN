# PathPoint クラスの概要

ソースコード: `Source/GUNMAN/Enemy/PathPoint.h / .cpp`

## 概要

`APathPoint` は `AActor` を継承した、敵の巡回ルートを定義するアクターです。  
`FVector` 型の配列 `PathPoint` に巡回座標を格納し、`BTT_TaskPath` がこの配列を順番に参照して敵を移動させます。  
`MakeEditWidget="true"` 属性により、Unreal エディタ上でビジュアルウィジェットを使って座標を直接配置できます。

## プロパティ

| プロパティ | 型 | 説明 |
|---|---|---|
| `PathPoint` | `TArray<FVector>` | 巡回ポイントの座標配列。エディタで直接編集可能 |

## 使われ方

```mermaid
flowchart LR
    P["APathPoint\nレベルに配置\nPathA / PathB タグを付与"]
    T["BTT_TaskPath::ExecuteTask"]
    E["AAIEnemy::index\n現在のインデックス"]

    T -->|"GetAllActorsOfClassWithTag(Tags[0])"| P
    P -->|"PathPoint[Enemy->index]"| T
    T --> E
    E -->|"インデックスを更新"| T
```

`BTT_TaskPath` は `GetAllActorsOfClassWithTag` でタグ一致の `BP_PathPoint` を検索し、  
`GetActorTransform().TransformPosition(PathPoint[Enemy->index])` でローカル座標をワールド座標に変換して移動先を決定します。
