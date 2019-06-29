using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEditor;

namespace WFC_CS.Editor
{
	/// <summary>
	/// A piece of the tileset editor window that edits a specific tile.
	/// </summary>
	public class Tileset3D_TileEditor : Tileset3DEditorPiece
	{
		public Tileset3D Tileset { get; private set; }
		public int TileIndex { get; private set; }

		public Tileset3D.Tile TileCopy { get; private set; }

		public bool VisualizeBounds = false,
					VisualizeAxes = false;


		private EditorWindowScene tileScene = new EditorWindowScene();
		private Transform tileSceneBoundsViz;
		private Transform[] tileSceneAxesViz = new Transform[3];
		private GameObject tileScenePrefabViz, tileScenePrefabSource;


		public Tileset3D_TileEditor() { }
		public Tileset3D_TileEditor(Tileset3D tileset, int tileIndex = 0)
		{
			Reset(tileset, tileIndex);
		}

		public void Reset(Tileset3D newTileset, int newTileIndex = 0)
		{
			tileScene.ClearScene();

			Tileset = newTileset;
			TileIndex = newTileIndex;
			if (Tileset.Tiles.Count < 1)
			{
				TileCopy = null;
				return;
			}

			TileCopy = new Tileset3D.Tile(Tileset.Tiles[TileIndex]);
			
			//Set up the preview scene.
			tileScene.CenterCameraOn(Tileset.TileBounds);

			//Set up the "bounds" visualization object.
			tileSceneBoundsViz = CreateUnitCube(Color.white.ChangeA(0.25f), VisualizeBounds,
												"Bounds Visualization");

			//Set up the axis visualizations.
			const float axisAlphas = 0.75f;
			tileSceneAxesViz[0] = CreateUnitCube(Color.red.ChangeA(axisAlphas),
												 VisualizeAxes, "X Axis");
			tileSceneAxesViz[1] = CreateUnitCube(Color.green.ChangeA(axisAlphas),
												 VisualizeAxes, "Y Axis");
			tileSceneAxesViz[2] = CreateUnitCube(Color.blue.ChangeA(axisAlphas),
												 VisualizeAxes, "Z Axis");

			//Run an update frame to initialize everything.
			UpdatePrefab();
			UpdateVizObjects();
		}
		public override void Dispose()
		{
			base.Dispose();
			tileScene.Dispose();
		}


		protected override string Description =>
			"tile " + ((TileCopy == null || TileCopy.Prefab == null) ? "" : TileCopy.Prefab.name);

		public override void SaveChanges()
		{
			Tileset.Tiles[TileIndex] = TileCopy;

			base.SaveChanges();
		}
		public override void RevertChanges()
		{
			base.RevertChanges();

			if (Tileset != null && Tileset.Tiles.Count > 0)
				TileCopy = new Tileset3D.Tile(Tileset.Tiles[TileIndex]);
			else
				TileCopy = null;
		}

		public override void DoGUILayout()
		{
			if (Tileset == null || TileCopy == null)
			{
				GUILayout.Label("No tile selected", TilesetGUI.Style_Label_Title);
				return;
			}

			//Draw the tile preview.
			var rect = GUILayoutUtility.GetRect(tileScene.Cam.targetTexture.width,
												tileScene.Cam.targetTexture.height);
			GUI.DrawTexture(rect, tileScene.Cam.targetTexture);

			//Now overlay some GUI stuff on that preview.
			using (GUIBlock.Layout_Area(rect))
			{
				GUILayout.FlexibleSpace();

				using (GUIBlock.Layout_Horizontal())
				{
					VisualizeBounds = EditorGUILayout.Toggle("Show Bounds", VisualizeBounds, 
															 TilesetGUI.Style_Checkbox);

					GUILayout.FlexibleSpace();

					VisualizeAxes = EditorGUILayout.Toggle("Show Axes", VisualizeAxes,
														   TilesetGUI.Style_Checkbox);
				}
			}

			//Lastly, update the scene preview.
			if (Event.current.type == EventType.Repaint)
			{
				UpdateVizObjects();
				UpdatePrefab();
				tileScene.Cam.Render();
			}

			base.DoGUILayout();
		}


		private void UpdatePrefab()
		{
			if (tileScenePrefabSource != TileCopy.Prefab)
			{
				//Clear out the old prefab visualization.
				if (tileScenePrefabViz != null)
					GameObject.DestroyImmediate(tileScenePrefabViz);
				tileScenePrefabViz = null;

				//Store the reference to the new prefab.
				tileScenePrefabSource = TileCopy.Prefab;
				if (tileScenePrefabSource == null)
					return;

				//Create an instance of the new prefab.
				tileScenePrefabViz = GameObject.Instantiate(tileScenePrefabSource,
															tileScene.Container,
															true);

				//Warn if the prefab has a non-zero starting position.
				//TODO: Option to disable this warning.
				if (tileScenePrefabViz.transform.position != Vector3.zero)
				{
					Debug.LogWarning("Prefab '" + tileScenePrefabSource.name +
									     "' doesn't have an initial position at {0,0,0}." +
									     " This offset will be used when placing the tile." +
									     " If that was not intentional, please zero out the position in the prefab.");
				}
			}
		}

		private Transform CreateUnitCube(Color col, bool isActive, string objName = "Unit Cube")
		{
			var tr = new GameObject(objName).transform;
			tr.SetParent(tileScene.Container);

			tr.position = Vector3.zero;
			tr.rotation = Quaternion.identity;
			tr.localScale = Vector3.one;

			tr.gameObject.AddComponent<MeshFilter>().sharedMesh = Resources.Load<Mesh>("UnitCube");

			var mr = tr.gameObject.AddComponent<MeshRenderer>();
			mr.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
			mr.receiveShadows = false;
			mr.sharedMaterial = Resources.Load<Material>("UnlitTransparentColor");
			mr.material.color = col;

			tr.gameObject.SetActive(isActive);

			return tr;
		}
		private void UpdateVizObjects()
		{
			//Update the bounds visualization.
			tileSceneBoundsViz.position = -(0.5f * Vector3.one) +
										  Tileset.TileBounds.center;
			tileSceneBoundsViz.rotation = Quaternion.identity;
			tileSceneBoundsViz.localScale = Tileset.TileBounds.size;
			tileSceneBoundsViz.gameObject.SetActive(VisualizeBounds);

			//Update the axes visualization.
			const float axisPerpendicularScale = 0.04f;
			for (int i = 0; i < 3; ++i)
			{
				var tr = tileSceneAxesViz[i];
				Vector3 pos = Vector3.zero,
						scale = Tileset.TileBounds.size * axisPerpendicularScale;

				pos[i] = Tileset.TileBounds.extents[i];
				scale[i] = Tileset.TileBounds.size[i];

				tr.position = pos;
				tr.localScale = scale;

				tr.gameObject.SetActive(VisualizeAxes);
			}
		}
	}
}