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
		//TODO: Camera and unit cubes have to be in a real scene, not the preview scene. Why is this?


		public Tileset3D Tileset { get; private set; }
		public int TileIndex { get; private set; }

		public Tileset3D.Tile TileCopy { get; private set; }

		public bool VisualizeBounds = false,
					VisualizeAxes = false;

		public int NewPreviewSize = 256;
		private int currentPreviewSize;
		private float previewScale { get { return (float)NewPreviewSize / currentPreviewSize; } }


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
			currentPreviewSize = NewPreviewSize;

			tileScene.ClearScene();
			tileScene.SetCameraRes(new Vector2Int(currentPreviewSize, currentPreviewSize));
			tileScene.Cam.fieldOfView = 80;
			tileScene.Cam.nearClipPlane = 0.01f;
			tileScene.Cam.farClipPlane = 10;

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
			tileSceneBoundsViz = CreateUnitCube(VisualizeBounds, "BoundsViz",
												"Bounds Visualization");

			//Set up the axis visualizations.
			tileSceneAxesViz[0] = CreateUnitCube(VisualizeAxes, "XAxisViz", "X Axis");
			tileSceneAxesViz[1] = CreateUnitCube(VisualizeAxes, "YAxisViz", "Y Axis");
			tileSceneAxesViz[2] = CreateUnitCube(VisualizeAxes, "ZAxisViz", "Z Axis");

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
				GUILayout.FlexibleSpace();
				using (GUIBlock.Layout_Horizontal())
				{
					GUILayout.FlexibleSpace();
					GUILayout.Label("No tile selected", TilesetGUI.Style_Label_Title);
					GUILayout.FlexibleSpace();
				}
				GUILayout.FlexibleSpace();
				return;
			}

			//Do the tile preview on the left side, and the data editor on the right side.
			using (GUIBlock.Layout_Horizontal())
			{
				using (GUIBlock.Layout_Vertical())
					DoGUITilePreview();

				GUILayout.FlexibleSpace();

				GUILayout.FlexibleSpace(); //Debug placeholder before tile data editors are made.
				base.DoGUILayout();
			}
			
			//Lastly, update the scene preview.
			if (Event.current.type == EventType.Repaint)
			{
				UpdateVizObjects();
				UpdatePrefab();
				
				tileScene.Cam.Render();
			}
		}
		
		private void DoGUITilePreview()
		{
			//Do the slider for texture size.
			int desiredPreviewSize = Mathf.RoundToInt(GUILayout.HorizontalSlider(
				NewPreviewSize, 256, 1024,
				TilesetGUI.Style_SliderBar, TilesetGUI.Style_SliderThumb));
			bool changedThisFrame = (desiredPreviewSize != NewPreviewSize),
				 wasChanging = (NewPreviewSize != currentPreviewSize);

			//If the view size is done being changed, update the camera texture resolution.
			if (wasChanging && !changedThisFrame)
			{
				tileScene.SetCameraRes(new Vector2Int(desiredPreviewSize, desiredPreviewSize));
				currentPreviewSize = desiredPreviewSize;

				tileScene.Cam.Render();
			}
			NewPreviewSize = desiredPreviewSize;

			//Draw the scene as a box.
			var camTex = tileScene.Cam.targetTexture;
			GUILayout.Box(camTex,
						  GUILayout.Width(camTex.width * previewScale),
						  GUILayout.Height(camTex.height * previewScale));

			using (GUIBlock.Layout_Horizontal())
			{
				VisualizeBounds = EditorGUILayout.Toggle("Show Bounds", VisualizeBounds,
														 TilesetGUI.Style_Checkbox);

				GUILayout.FlexibleSpace();

				VisualizeAxes = EditorGUILayout.Toggle("Show Axes", VisualizeAxes,
													   TilesetGUI.Style_Checkbox);
			}
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

		private Transform CreateUnitCube(bool isActive, string materialName,
										 string objName = "Unit Cube")
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
			mr.sharedMaterial = Resources.Load<Material>(materialName);

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