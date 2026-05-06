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
    # Blenderクラスを登録
    for cls in classes:
        bpy.utils.register_class(cls)

    # 項目の追加
    bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_my_menu.submenu)
    print("レベルエディタが有効化されました。")

def unregister():
    # 項目の削除
    bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_my_menu.submenu)
    
    # Blenderからクラスの削除
    for cls in classes:
        bpy.utils.unregister_class(cls)
    print("レベルエディタが無効化されました。")

# メニュー項目
def draw_menu_manual(self, context):
    # トップバーのエディターメニューに項目を追加
    self.layout.operator("wm.url_open_preset", text="Manual", icon='HELP')

#トップバーの拡張メニュー
class TOPBAR_MT_my_menu(bpy.types.Menu):
    # Blenderがクラスを識別するための固有文字列
    bl_idname = "TOPBAR_MT_my_Menu"
    # メニューのラベルとして表示される文字列
    bl_label = "MyMenu"
    # 著作者表示
    bl_description = "拡張メニュー by " + bl_info["author"]

    # サブメニューの描画
    def draw(self, context):
        # トップバーのエディターメニューに項目を追加
        self.layout.operator("wm.url_open_preset", text="Manual", icon='HELP')
        self.layout.operator("wm.url_open_preset", text="separator0", icon='HELP')
        self.layout.separator()
        self.layout.operator("wm.url_open_preset", text="separator1", icon='HELP')

    #既存のメニューにサブメニューを追加
    def submenu(self, context):
        # ID指定でサブメニューを追加
        self.layout.menu(TOPBAR_MT_my_menu.bl_idname)

# Blenderに登録するクラスリスト
classes = (
    TOPBAR_MT_my_menu,
)