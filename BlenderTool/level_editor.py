import bpy
import math
import bpy_extras

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
class MYADDON_OT_export_scene(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
    bl_idname = "myaddon.myaddon_ot_export_scene"
    bl_label = "シーン出力"
    bl_description = "シーン情報をエクスポートします"

    #出力するファイルの拡張子
    filename_ext = ".scene"

    #メニューを実行したときに呼ばれるコールバック
    def execute(self,context):

        print("シーン情報をエクスポートします")

        #ファイルを出力
        self.export()

        print("シーン情報をエクスポートしました")
        self.report({'INFO'},"シーン情報をエクスポートしました")

        #オペレータの命令終了を通知
        return {'FINISHED'}
    
    def export(self):
        """ファイルに出力"""

        print("シーン情報出力開始... %r" % self.filepath)

        #ファイルをテキスト形式で書き出し用にオープン
        #スコープを抜けると自動的にクローズされる
        with open(self.filepath, "wt") as file:

            #ファイルに文字列を書き込む
            file.write("SCENE\n")

            #シーン内の全オブジェクトについて
            for object in bpy.context.scene.objects:
               
                if object.parent:
                    continue

                self.parse_scene_recursive(file, object, 0)

    def write_and_print(self,file,str):
        print(str)

        file.write(str)
        file.write('\n')

    def parse_scene_recursive(self, file, object, level):
        """シーン解析用再帰関数"""

        #深さ分インシデントする
        indent = ''
        for i in range(level):
            indent += "\t"

        #オブジェクト名書き込み
        self.write_and_print(file, indent + object.type + " - " + object.name)
        #ローカルトランスフォーム行列から平行移動、回転、スケールを抽出
        #型はVector,Quaternion,Vector
        trans,rot,scale, = object.matrix_local.decompose()
        #回転をQuternionからEuler(3軸での回転)に変換
        rot.x = math.degrees(rot.x)
        rot.y = math.degrees(rot.y)
        rot.z = math.degrees(rot.z)
        #トランスフォーム情報を表示
        self.write_and_print(file, indent + "Trans(%f,%f,%f)" % (trans.x, trans.y, trans.z))
        self.write_and_print(file, indent + "Rot(%f,%f,%f)" % (rot.x, rot.y, rot.z))
        self.write_and_print(file, indent + "Scale(%f,%f,%f)" % (scale.x, scale.y, scale.z))
        self.write_and_print(file, '')

        #子ノードへ進む
        for child in object.children:
            self.parse_scene_recursive(file, child, level + 1)



# Blenderに登録するクラスリスト
classes = (
    MYADDON_OT_stretch_vertex,
    MYADDON_OT_create_ico_sphere,
    MYADDON_OT_export_scene,
    TOPBAR_MT_my_menu,
)