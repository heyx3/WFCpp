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

		public string DisplayName
		{
			get
			{
				if (TileCopy.Prefab != null)
					return TileCopy.Prefab.name;
				if (Tileset.Tiles[TileIndex].Prefab != null)
					return Tileset.Tiles[TileIndex].Prefab.name;

				return "[no name]";
			}
		}


		public event Action<Tileset3D_TileEditor> OnDeleteButton;


		public bool VisualizeBounds = true,
					VisualizeAxes = true;

		public int NewPreviewSize = 512;
		private int currentPreviewSize;
		private float previewScale { get { return (float)NewPreviewSize / currentPreviewSize; } }

		private EditorWindowScene tileScene = new EditorWindowScene();
		private Transform tileSceneBoundsViz;
		private Transform[] tileSceneAxesViz = new Transform[3];
		private GameObject tileScenePrefabViz, tileScenePrefabSource;
		
		private Tileset3D_TileSymmetries pane_symmetries
		{
			get { return (Tileset3D_TileSymmetries)Children[0]; }
		}
		private Tileset3D_TileFaces pane_faces
		{
			get { return (Tileset3D_TileFaces)Children[1]; }
		}
		private int tabIndex
		{
			get { return Children.IndexOf(t => !t.IsDisabled); }
			set
			{
				for (int i = 0; i < Children.Count; ++i)
					Children[i].IsDisabled = (i != value);
			}
		}


		public Tileset3D_TileEditor()
			: base(new Tileset3DEditorPiece[] { new Tileset3D_TileSymmetries(), new Tileset3D_TileFaces() })
		{

		}
		public Tileset3D_TileEditor(Tileset3D tileset, int tileIndex = 0)
			: this()
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
			tileScene.Cam.farClipPlane = 100;

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


		public override string Description =>
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

				using (GUIBlock.Layout_Vertical())
				{
					int selectedTabI = this.tabIndex;
					for (int i = 0; i < Children.Count; ++i)
					{
						//Do the toggle and track whether it was used.
						Action ifTabSelected = () => this.tabIndex = i;
						using (GUIBlock.ChangeCheck(ifTabSelected))
						{
							GUILayout.Toggle(i == selectedTabI,
											 Children[i].Description,
											 TilesetGUI.Style_Button_Tab);
						}
					}
				}

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
			using (TrackChanges(UpdatePrefab))
			{
				TileCopy.Prefab = (GameObject)EditorGUILayout.ObjectField(TileCopy.Prefab,
																		  typeof(GameObject), true);
			}

			var sliderLayout = new GUILayoutOption[] {
				GUILayout.MinWidth(200),
				GUILayout.ExpandWidth(true)
			};
			var sliderLabelLayout = new GUILayoutOption[] {
				GUILayout.ExpandWidth(false)
			};

			//Do the slider for texture size.
			int desiredPreviewSize;
			using (GUIBlock.Layout_Horizontal())
			{
				GUILayout.Label("Preview Scale", TilesetGUI.Style_Label_Normal,
								sliderLabelLayout);
				desiredPreviewSize = Mathf.RoundToInt(GUILayout.HorizontalSlider(
			        NewPreviewSize, 256, 1024,
					TilesetGUI.Style_SliderBar, TilesetGUI.Style_SliderThumb,
					sliderLayout));
			}
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

			//Sliders for camera control.
			//TODO: Any way to control the camera with mouse dragging? Or at least arrow keys?
			var camTr = tileScene.Cam.transform;
			var currentAngles = camTr.eulerAngles;
			float currentZoom = (camTr.position - Tileset.TileBounds.center).magnitude /
								Tileset.TileBounds.GetBoundingRadius();
			var newAngles = new Vector3();
			float newZoom = 0;
			bool camAngleChanged = false;
			using (GUIBlock.ChangeCheck(() => camAngleChanged = true))
			{
				using (GUIBlock.Layout_Horizontal())
				{
					GUILayout.Label("Yaw", TilesetGUI.Style_Label_Normal, sliderLabelLayout);
					newAngles.y = GUILayout.HorizontalSlider(currentAngles.y, -720, 720,
															 TilesetGUI.Style_SliderBar,
															 TilesetGUI.Style_SliderThumb,
															 sliderLayout);
				}
				using (GUIBlock.Layout_Horizontal())
				{
					GUILayout.Label("Pitch", TilesetGUI.Style_Label_Normal, sliderLabelLayout);

					//The correct range for pitch is from 270.0001 to 89.9999, wrapping around to 0 at 360.
					//We have to do some extra work to make this into a sane slider.
					if (currentAngles.x < 270)
						currentAngles.x += 360;
					newAngles.x = GUILayout.HorizontalSlider(currentAngles.x, 270.1f, 360 + 89.9f,
															 TilesetGUI.Style_SliderBar,
															 TilesetGUI.Style_SliderThumb,
															 sliderLayout);
					if (newAngles.x > 360)
						newAngles.x -= 360;
				}
				using (GUIBlock.Layout_Horizontal())
				{
					GUILayout.Label("Zoom", TilesetGUI.Style_Label_Normal, sliderLabelLayout);
					newZoom = GUILayout.HorizontalSlider(currentZoom, 0.001f, 5,
														 TilesetGUI.Style_SliderBar,
														 TilesetGUI.Style_SliderThumb,
														 sliderLayout);
				}
			}
			//Re-center the rotation values if they're not currently being manipulated.
			if (!camAngleChanged)
			{
				if (newAngles.x < 0)
					newAngles.x += 360;
				if (newAngles.x >= 360)
					newAngles.x -= 360;
			}
			//Update the camera with the new settings.
			camTr.position = Tileset.TileBounds.center +
							 (Quaternion.Euler(newAngles) *
							    new Vector3(0, 0, newZoom * Tileset.TileBounds.GetBoundingRadius()));
			camTr.forward = (Tileset.TileBounds.center - camTr.position).normalized;

			//Visualization features.
			using (GUIBlock.Layout_Horizontal())
			{
				VisualizeBounds = GUILayout.Toggle(VisualizeBounds, "Show Bounds",
												   TilesetGUI.Style_Checkbox);

				GUILayout.Space(50);

				VisualizeAxes = GUILayout.Toggle(VisualizeAxes, "Show Axes",
												 TilesetGUI.Style_Checkbox);

				GUILayout.FlexibleSpace();
			}

			//Draw the scene.
			var camTex = tileScene.Cam.targetTexture;
			var camRect = GUILayoutUtility.GetAspectRect(camTex.width / (float)camTex.height,
													     GUILayout.MaxWidth(camTex.width * previewScale),
													     GUILayout.MaxHeight(camTex.height * previewScale));
			GUI.DrawTexture(camRect, tileScene.Cam.targetTexture);

			//The Save/Revert/Delete buttons.
			using (GUIBlock.Layout_Horizontal())
			{
				if (HasUnsavedChanges)
				{
					if (GUILayout.Button("Save", TilesetGUI.Style_Button_Normal))
						SaveChanges();
					if (GUILayout.Button("Revert", TilesetGUI.Style_Button_Normal))
						RevertChanges();
				}
				if (GUILayout.Button("Delete", TilesetGUI.Style_Button_Normal))
				{
					if (EditorUtility.DisplayDialog("Confirm Deleting Tile",
													"Are you sure you want to delete tile \"" + DisplayName + "\"?",
													"Yes", "No"))
					{
						OnDeleteButton?.Invoke(this);
					}
				}
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
			tileSceneBoundsViz.position = -(0.5f * Tileset.TileBounds.size) +
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
				
				scale[i] = Mathf.Max(1, Tileset.TileBounds.max[i]);

				tr.position = pos;
				tr.localScale = scale;

				tr.gameObject.SetActive(VisualizeAxes);
			}
		}
	}
}