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

		public override string Description => "the tileset";

		/// <summary>
		/// The child pane for editing a specific tile.
		/// </summary>
		private Tileset3D_TileEditor pane_tile
		{
			get { return (Tileset3D_TileEditor)Children[0]; }
		}


		public Tileset3D_Header()
			: base(new Tileset3DEditorPiece[] { new Tileset3D_TileEditor() })
		{
			pane_tile.Parent = this;
			pane_tile.OnDeleteButton += Callback_DeleteTileInPanel;
		}
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
			tilesetCopy.name = tilesetOriginal.name; //Remove the "(Clone)" that Unity appends to it.
		}
		
		public override void DoGUILayout()
		{
			using (GUIBlock.Layout_Horizontal())
			{
				//The title pane.
				using (GUIBlock.Layout_Vertical(GUILayout.Width(150)))
				{
					GUILayout.Label("WFC Tile3D", TilesetGUI.Style_Label_Title);
					if (tilesetCopy != null)
						GUILayout.Label(tilesetCopy.name, TilesetGUI.Style_Label_Normal);
				}

				MyGUILayout.DrawTexture(TilesetGUI.Tex_WhitePixel,
										width: 10, color: Color.black);

				//The "New", "Save", "Revert", and "Load" buttons.
				using (GUIBlock.Layout_Vertical(GUILayout.Width(80)))
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

							AssetDatabase.CreateAsset(newTileset, savePath);

							Reset(newTileset);
						}
					}

					if (GUILayout.Button("Load", TilesetGUI.Style_Button_Normal) &&
						ConfirmClosing(true) != ConfirmClosingDialog.Results.Cancel)
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
							using (GUIBlock.Enabled(pane_tile.TileIndex != tileI))
							{
								//Get the button's label from the prefab's name.
								var prefab = tilesetCopy.Tiles[tileI].Prefab;
								string name = (prefab == null ? "[no prefab]" : prefab.name);

								if (GUILayout.Button(name, TilesetGUI.Style_Button_Normal) &&
									pane_tile.ConfirmClosing(true) != ConfirmClosingDialog.Results.Cancel)
								{
									pane_tile.Reset(tilesetCopy, tileI);
								}
							}
						}
					}
					if (GUILayout.Button("+", TilesetGUI.Style_Button_BigText, GUILayout.ExpandWidth(false)))
						if (pane_tile.ConfirmClosing(true) != ConfirmClosingDialog.Results.Cancel)
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
					tilesetCopy.TileBounds.size = Vector3.Max(tilesetCopy.TileBounds.size,
															  Vector3.one * 0.00001f);
				}
			}

			//Do child GUI underneath.
			//Leave a bit of space on the sides and bottom.
			MyGUILayout.DrawTexture(TilesetGUI.Tex_WhitePixel,
									height: 10, color: Color.black);
			using (GUIBlock.Layout_HorizontalPad(10))
				base.DoGUILayout();
			GUILayout.FlexibleSpace();
		}

		private void Callback_DeleteTileInPanel(Tileset3D_TileEditor pane)
		{
			int toDeleteI = pane_tile.TileIndex;
			pane_tile.Reset(tilesetCopy);

			tilesetCopy.Tiles.RemoveAt(toDeleteI);
			HasUnsavedChanges = true;
		}
	}
}
