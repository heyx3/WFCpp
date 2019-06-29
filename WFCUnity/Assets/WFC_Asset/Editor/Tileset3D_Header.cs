using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using UnityEngine;
using UnityEditor;


namespace WFC_CS.Editor
{
	/// <summary>
	/// The root of the tileset editor pane tree.
	/// </summary>
	public class Tileset3D_Header : Tileset3DEditorPiece
	{
		private Tileset3D tilesetCopy;
		private Tileset3D tilesetOriginal;

		private Vector2 tileSelectionScrollPos = Vector2.zero;

		protected override string Description => "the tileset";

		/// <summary>
		/// The child pane for editing a specific tile.
		/// </summary>
		private Tileset3D_TileEditor pane_tile
		{
			get { return (Tileset3D_TileEditor)Children[0]; }
			set { Children[0] = value; }
		}


		public Tileset3D_Header() { Children.Add(new Tileset3D_TileEditor()); }
		public Tileset3D_Header(Tileset3D original) : this() { Reset(original); }

		public void Reset(Tileset3D tileset)
		{
			tilesetOriginal = tileset;

			RevertChanges();
			pane_tile.Reset(tilesetCopy);
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
		
		public override void DoGUILayout()
		{
			using (GUIBlock.Layout_Horizontal())
			{
				//The title pane.
				using (GUIBlock.Layout_Vertical(GUILayout.ExpandWidth(false)))
				{
					GUILayout.Label("WFC Tile3D", TilesetGUI.Style_Label_Title);
					if (tilesetCopy != null)
						GUILayout.Label(tilesetCopy.name, TilesetGUI.Style_Label_Normal);
				}

				MyGUILayout.DrawTexture(TilesetGUI.Tex_WhitePixel,
										width: 10, color: Color.black);

				//The "New", "Save", "Revert", and "Load" buttons.
				using (GUIBlock.Layout_Vertical())
				{
					//Buttons that are always available:
					if (GUILayout.Button("New", TilesetGUI.Style_Button_Normal))
					{
						string savePath = EditorUtility.SaveFilePanelInProject(
							"Choose location", "NewTileset", "asset",
							"Choose where to save the new tileset");
						if (!string.IsNullOrEmpty(savePath))
						{
							var newTileset = ScriptableObject.CreateInstance<Tileset3D>();
							newTileset.name = Path.GetFileNameWithoutExtension(savePath);

							//TODO: How to overwrite?
							AssetDatabase.CreateAsset(newTileset, savePath);

							Reset(newTileset);
						}
					}

					if (GUILayout.Button("Load", TilesetGUI.Style_Button_Normal))
					{
						string loadPath = EditorUtility.OpenFilePanel("Load tileset",
																      Application.dataPath,
																	  "asset");
						if (!string.IsNullOrEmpty(loadPath))
						{
							if (!loadPath.IsPathInsideFolder(Application.dataPath))
							{
								Debug.LogError("You chose an asset that's not inside this project.");
							}
							else
							{
								loadPath = loadPath.MakeRelativePath("Assets");
								var tileset = AssetDatabase.LoadAssetAtPath<Tileset3D>(loadPath);
								Reset(tileset);
							}
						}
					}
					
					//Buttons that are only available if there are unsaved changes.
					using (GUIBlock.Enabled(HasUnsavedChanges_Recursive))
					{
						if (GUILayout.Button("Save", TilesetGUI.Style_Button_Normal))
						{
							SaveChanges();
						}
						if (GUILayout.Button("Revert", TilesetGUI.Style_Button_Normal))
						{
							RevertChanges();
						}
					}
				}
				
				MyGUILayout.DrawTexture(TilesetGUI.Tex_WhitePixel,
										width: 10, color: Color.black);

				//The tile selector.
				if (tilesetCopy == null)
				{
					GUILayout.FlexibleSpace();
				}
				else
				{
					using (GUIBlock.Layout_Scroll(ref tileSelectionScrollPos))
					{
						for (int tileI = 0; tileI < tilesetCopy.Tiles.Count; ++tileI)
						{
							string name = tilesetCopy.Tiles[tileI].Prefab.name;
							using (GUIBlock.Enabled(pane_tile.TileIndex != tileI))
							{
								if (GUILayout.Button(name, TilesetGUI.Style_Button_Normal) &&
									pane_tile.ConfirmClosing(true) != ConfirmClosingDialog.Results.Cancel)
								{
									pane_tile.Reset(tilesetCopy, tileI);
								}
							}
						}
					}
					if (GUILayout.Button("+", TilesetGUI.Style_Button_BigText) &&
						pane_tile.ConfirmClosing(true) != ConfirmClosingDialog.Results.Cancel)
					{
						HasUnsavedChanges = true;

						tilesetCopy.Tiles.Add(new Tileset3D.Tile());
						pane_tile.Reset(tilesetCopy, tilesetCopy.Tiles.Count - 1);
					}
				}
				
				MyGUILayout.DrawTexture(TilesetGUI.Tex_WhitePixel,
										width: 10, color: Color.black);

				if (tilesetCopy == null)
				{
					GUILayout.FlexibleSpace();
				}
				else using (TrackChanges())
				{
					//TODO: GUIStyle?
					tilesetCopy.TileBounds = EditorGUILayout.BoundsField("Bounds",
																		 tilesetCopy.TileBounds);
				}
			}

			//Do child GUI underneath.
			MyGUILayout.DrawTexture(TilesetGUI.Tex_WhitePixel,
									height: 10, color: Color.black);
			GUILayout.FlexibleSpace();
			base.DoGUILayout();

			//Leave a bit of space on the bottom.
			GUILayout.Space(10.0f);
		}
	}
}
