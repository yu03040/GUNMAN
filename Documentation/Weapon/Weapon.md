# Weapon クラスの概要

## 主な処理内容

`Weapon` クラスは、すべての武器の基底クラスとして機能するクラスです。このクラスでは、武器の基本的な設定や機能が実装されています。

- **コンストラクターでの処理**: クラスのコンストラクターで、プロパティのデフォルト設定を行い、武器に関連するデータテーブルをロードします。
- **武器のアタッチ**: プレイヤーキャラクターに対して武器をアタッチする機能が実装されています。敵キャラクター (AIEnemy クラス) には敵側でのアタッチが行われますが、プレイヤーにはこのクラス内で処理が行われます。
- **武器の非表示**: 初期状態で武器が表示されないようにし、装備時にのみ表示される設定がなされています。

## このクラスのソースコードの説明

### コンストラクター (`AWeapon::AWeapon`)

- **DefaultSceneRoot**: `USceneComponent` のサブオブジェクトを作成し、`RootComponent` に設定しています。これが武器のルートコンポーネントとなり、武器全体の基盤となります。

- **WeaponMesh**: `USkeletalMeshComponent` を作成し、`DefaultSceneRoot` にアタッチしています。これにより、武器のメッシュが設定され、ビジュアルの表示が可能となります。

- **データテーブルのロード**: `ConstructorHelpers::FObjectFinder` を使用して、`DT_Weapon` という名前のデータテーブルをロードしています。このデータテーブルには、武器に関する設定情報が含まれています。

### `BeginPlay` 関数

- **WeaponMeshの非表示設定**: ゲーム開始時 (`BeginPlay`) に武器メッシュを非表示 (`SetHiddenInGame(true, true)`) に設定します。これにより、武器は装備されるまで画面に表示されません。

- **データテーブルからの情報取得**: データテーブルの行を取得し、その情報に基づいてプレイヤーに武器をアタッチします。具体的には、`WeaponMesh` の `ComponentTags` に設定されたタグを基に行を検索し、対応するアタッチポイントに武器をアタッチします。

- **プレイヤーキャラクターとのインターフェース呼び出し**: プレイヤーキャラクターが `IWeaponInterface` を実装している場合、そのインターフェースを介して武器をプレイヤーにアタッチします。このとき、`WeaponMesh` とデータテーブルから取得したソケット名を使用して、武器を適切な場所に取り付けます。