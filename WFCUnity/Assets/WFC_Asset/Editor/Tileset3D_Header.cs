using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEditor;


namespace WFC_CS.Editor
{
	public class Tileset3D_Header : Tileset3DEditorPiece
	{
		private Tileset3D tilesetCopy;
		private Tileset3D tilesetOriginal;

		private Vector2 tileSelectionScrollPos = Vector2.zero;

		protected override string Description => "the tileset";

		private Tileset3D_TileEditor pane_tile
		{
			get { return (Tileset3D_TileEditor)Children[0]; }
			set { Children[0] = value; }
		}


		public Tileset3D_Header() { Children.Add(new Tileset3D_TileEditor()); }
		public Tileset3D_Header(Tileset3D original)
		{
			tilesetOriginal = original;
			Children.Add(new Tileset3D_TileEditor(tilesetCopy));
		}


		public override void SaveChanges()
		{
			EditorUtility.CopySerialized(tilesetCopy, tilesetOriginal);
			base.SaveChanges();
		}
		public override void RevertChanges()
		{
			base.RevertChanges();
			tilesetCopy = UnityEngine.Object.Instantiate(tilesetOriginal);
		}
		
		public void DoGUILayout()
		{
			using (GUIBlock.Layout_Horizontal())
			{
				//The title pane.
				using (GUIBlock.Layout_Vertical())
				{
					GUILayout.Label("WFC Tile3D", TilesetGUI.Style_Label_Title);
					GUILayout.Label("WFC Tile3D", TilesetGUI.Style_Label_Normal);
				}

				MyGUILayout.DrawTexture(TilesetGUI.Tex_WhitePixel,
										width: 10, color: Color.black);

				//TODO: The "New", "Save", "Revert", and "Load" buttons.

				//The tile selector.
				using (GUIBlock.Layout_Scroll(ref tileSelectionScrollPos))
				{
					for (int tileI = 0; tileI < tilesetCopy.Tiles.Count; ++tileI)
					{
						string name = tilesetCopy.Tiles[tileI].Prefab.name;
						using (GUIBlock.Enabled(pane_tile.Data.TileIndex != tileI))
						{
							if (GUILayout.Button(name, TilesetGUI.Style_Button_Normal) &&
								pane_tile.TryToClose(true) != ConfirmClosingDialog.Results.Cancel)
							{
								pane_tile = new Tileset3D_TileEditor(tilesetCopy, tileI);
							}
						}
					}
				}
				if (GUILayout.Button("+", TilesetGUI.Style_Button_BigText) &&
					pane_tile.TryToClose(true) != ConfirmClosingDialog.Results.Cancel)
				{
					tilesetCopy.Tiles.Add(new Tileset3D.Tile());
					pane_tile = new Tileset3D_TileEditor(tilesetCopy,
														 tilesetCopy.Tiles.Count - 1);
				}
				
				MyGUILayout.DrawTexture(TilesetGUI.Tex_WhitePixel,
										width: 10, color: Color.black);

				//TODO: Edit fields like "Bounds".
			}
		}
	}
}
