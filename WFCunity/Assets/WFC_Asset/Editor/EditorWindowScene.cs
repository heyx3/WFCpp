using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;
using UnityEditor.SceneManagement;


namespace WFC_CS.Editor
{
	/// <summary>
	/// A scene meant to be used in an editor window,
	///     and handled separately from all the real game scenes.
	/// </summary>
	public class EditorWindowScene : IDisposable
	{
		public Scene TheScene { get; private set; }

		/// <summary>
		/// The camera in the scene.
		/// </summary>
		public Camera Cam { get; private set; }
		/// <summary>
		/// The object that other scene objects should be attached to.
		/// </summary>
		public Transform Container { get; private set; }


		public EditorWindowScene()
		{
			TheScene = EditorSceneManager.NewPreviewScene();

			//Add a camera and an empty container to the scene.
			
			Func<string, GameObject> createEmpty = name =>
			{
				var obj = (GameObject)PrefabUtility.InstantiatePrefab(Resources.Load("Empty Prefab"), TheScene);
				obj.name = name;
				return obj;
			};
			Cam = createEmpty("Camera").AddComponent<Camera>();
			Container = createEmpty("Container").transform;

			SetCameraRes(new Vector2Int(256, 256));
		}
		public void Dispose()
		{
			//Clean up the camera's render target first.
			if (Cam.targetTexture != null)
			{
				//Avoid an internal Unity warning message.
				if (RenderTexture.active == Cam.targetTexture)
					RenderTexture.active = null;

				Cam.targetTexture.Release();
				Cam.targetTexture = null;
			}

			//Finally, close the preview scene.
			EditorSceneManager.ClosePreviewScene(TheScene);
			TheScene = new Scene();
		}


		/// <summary>
		/// Sets the resolution of the camera's render texture if it isn't already.
		/// Automatically cleans up the old texture if a new one has to be allocated.
		/// </summary>
		public void SetCameraRes(Vector2Int res)
		{
			var currentTex = Cam.targetTexture;

			if (currentTex != null)
			{
				if (currentTex.width == res.x && currentTex.height == res.y)
					return;
				else
				{
					//Avoid an internal Unity warning message.
					if (RenderTexture.active == currentTex)
						RenderTexture.active = null;

					currentTex.Release();
				}
			}

			currentTex = new RenderTexture(res.x, res.y, 24, RenderTextureFormat.ARGB32);
			currentTex.Create();
			Cam.targetTexture = currentTex;
		}
		/// <summary>
		/// Removes all objects parented to "Container".
		/// </summary>
		public void ClearScene()
		{
			//Fun fact: the original version of this was basically
			//    while (tr.childCount > 0) DestroyImmediate(child)
			//  but it was an infinite loop because childCount never actually updates.
			foreach (var childTr in Container.Cast<Transform>().ToList())
				GameObject.DestroyImmediate(childTr.gameObject);
		}
		/// <summary>
		/// Centers the camera around the given bounds.
		/// </summary>
		/// <param name="distanceScale">
		/// How far the camera should be from the bounding box,
		///     as a multiple of the bounding box's radius.
		/// </param>
		public void CenterCameraOn(Bounds b, float distanceScale = 2.5f)
		{
			//Get the bounding sphere radius for this box.
			Vector3 bSize = b.size;
			float boundsRadius = Mathf.Sqrt((bSize.x * bSize.x) +
											(bSize.y * bSize.y) +
											(bSize.z * bSize.z));

			//Push the camera forward or backward to be at that radius.
			Vector3 camPosDir = (Cam.transform.position - b.center).normalized;
			Cam.transform.position = b.center + (camPosDir * boundsRadius * distanceScale);

			//Point the camera at the bounds.
			if (Cam.transform.up != -camPosDir)
				Cam.transform.forward = -camPosDir;
		}
	}
}
