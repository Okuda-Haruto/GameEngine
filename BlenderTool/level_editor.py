import bpy
import math

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
        self.layout.separator()

        self.layout.operator(MYADDON_OT_stretch_vertex.bl_idname, text=MYADDON_OT_stretch_vertex.bl_label)
        self.layout.operator(MYADDON_OT_create_ico_sphere.bl_idname, text=MYADDON_OT_create_ico_sphere.bl_label)
        self.layout.operator(MYADDON_OT_export_scene.bl_idname, text=MYADDON_OT_export_scene.bl_label)

    #既存のメニューにサブメニューを追加
    def submenu(self, context):
        # ID指定でサブメニューを追加
        self.layout.menu(TOPBAR_MT_my_menu.bl_idname)

#オペレータ 頂点を伸ばす
class MYADDON_OT_stretch_vertex(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_stretch_vertex"
    bl_label = "頂点を伸ばす"
    bl_description = "頂点座標を引っ張って伸ばします"
    #redo,undo可能オプション
    bl_options = {'REGISTER', 'UNDO'}

    #メニューを実行したときに呼ばれるコールバック
    def execute(self,context):
        bpy.data.objects["Cube"].data.vertices[0].co.x += 1.0
        print("頂点を伸ばしました。")

        #オペレータの命令終了を通知
        return {'FINISHED'}
    
#オペレータ ICO球生成
class MYADDON_OT_create_ico_sphere(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_object"
    bl_label = "ICO球生成"
    bl_description = "ICO球を生成します"
    #redo,undo可能オプション
    bl_options = {'REGISTER', 'UNDO'}

    #メニューを実行したときに呼ばれるコールバック
    def execute(self,context):
        bpy.ops.mesh.primitive_ico_sphere_add()
        print("ICO球を生成しました。")

        #オペレータの命令終了を通知
        return {'FINISHED'}
#オペレータ シーン出力
class MYADDON_OT_export_scene(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_export_scene"
    bl_label = "シーン出力"
    bl_description = "シーン情報をエクスポートします"

    #メニューを実行したときに呼ばれるコールバック
    def execute(self,context):

        print("シーン情報をエクスポートします")

        #シーン内の全オブジェクトについて
        for object in bpy.context.scene.objects:
            print(object.type + " - " + object.name)
            #ローカルトランスフォーム行列から平行移動、回転、スケールを抽出
            #型はVector,Quaternion,Vector
            trans,rot,scale, = object.matrix_local.decompose()
            #回転をQuternionからEuler(3軸での回転)に変換
            rot.x = math.degrees(rot.x)
            rot.y = math.degrees(rot.y)
            rot.z = math.degrees(rot.z)
            #トランスフォーム情報を表示
            print("Trans(%f,%f,%f)" % (trans.x, trans.y, trans.z))
            print("Rot(%f,%f,%f)" % (rot.x, rot.y, rot.z))
            print("Scale(%f,%f,%f)" % (scale.x, scale.y, scale.z))
            #親オブジェクトの名前を表示
            if object.parent:
                print("Parent:" + object.parent.name)

        print("シーン情報をエクスポートしました")
        self.report({'INFO'},"シーン情報をエクスポートしました")

        #オペレータの命令終了を通知
        return {'FINISHED'}

# Blenderに登録するクラスリスト
classes = (
    MYADDON_OT_stretch_vertex,
    MYADDON_OT_create_ico_sphere,
    MYADDON_OT_export_scene,
    TOPBAR_MT_my_menu,
)