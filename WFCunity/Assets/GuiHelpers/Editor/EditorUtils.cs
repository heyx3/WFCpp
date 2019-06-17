using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using UnityEngine;
using UnityEditor;


public static class EditorUtils
{
	[MenuItem("Assets/Clean Up Unused Assets")]
	public static void CleanUpSceneLeaks()
	{
		Resources.UnloadUnusedAssets();
	}
}