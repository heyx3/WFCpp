using UnityEngine;
using UnityEditor;

namespace WFC_CS.Utils
{
	public static class ConfirmClosingDialog
	{
		public enum Results
		{
			Save = 0,
			DontSave = 1,
			Cancel = 2,
		}
		public static Results Show(bool allowCancel,
								   string title = "Unsaved Changes",
								   string msg = "You have unsaved changes. What do you want to do?")
		{
			if (allowCancel)
				return (Results)EditorUtility.DisplayDialogComplex(title, msg, "Save", "Don't Save", "Cancel");
			else
				return EditorUtility.DisplayDialog(title, msg, "Save", "Don't Save") ? Results.Save : Results.DontSave;
		}
	}
}