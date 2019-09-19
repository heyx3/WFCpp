using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using UnityEngine;
using UnityEditor;

namespace WFC_CS.Utils
{
	public static class EditorUtils
	{
		[MenuItem("Assets/Clean Up Unused Assets")]
		public static void CleanUpSceneLeaks()
		{
			Resources.UnloadUnusedAssets();
		}
	}
}