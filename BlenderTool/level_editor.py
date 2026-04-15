import bpy

# ブレンダーに登録するアドオン情報
bl_info = {
    "name": "レベルエディタ",
    "author": "Haruto Okuda",
    "version": (1, 0),
    "blender": (3, 3, 1),
    "description": "レベルエディタ",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Object",
}

def register():
    print("レベルエディタが有効化されました。")

def unregister():
    print("レベルエディタが無効化されました。")

# テスト実行用コード
if __name__ == "__main__":
    register()