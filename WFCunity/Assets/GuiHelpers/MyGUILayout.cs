using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;


public static class MyGUILayout
{
	public static void TabIn(float preSpace = 20.0f)
	{
		BeginCompact(preSpace);
		GUILayout.BeginVertical();
	}
	public static void TabOut(float postSpace = 0.0f)
	{
		GUILayout.EndVertical();
		EndCompact(postSpace);
	}

	public static void BeginCompact(float frontSpace = 0.0f)
	{
		GUILayout.BeginHorizontal();

		if (frontSpace < 0)
			GUILayout.FlexibleSpace();
		else if (frontSpace > 0)
			GUILayout.Space(frontSpace);
	}
	public static void EndCompact(float endSpace = -1.0f)
	{
		if (endSpace < 0)
			GUILayout.FlexibleSpace();
		else if (endSpace > 0)
			GUILayout.Space(endSpace);

		GUILayout.EndHorizontal();
	}

	public static bool Foldout(ref bool value, string label = null)
	{
		//Don't change the result of GUI.changed here.
		var oldChangedValue = GUI.changed;

		BeginCompact();

		if (label != null)
			GUILayout.Label(label);

		if (GUILayout.Button(value ? "^" : "v"))
			value = !value;

		EndCompact();

		GUI.changed = oldChangedValue;

		return value;
	}
	
	public static void DrawTexture(Texture tex,
								   ScaleMode scaleMode = ScaleMode.StretchToFill,
								   bool alphaBlend = false,
								   float minLength = 0,
								   float maxLength = float.PositiveInfinity,
								   Color? color = null,
								   float scale = 1.0f, Vector2? scale2D = null)
	{
		if (!color.HasValue)
			color = Color.white;
		if (!scale2D.HasValue)
			scale2D = Vector2.one;

		Vector2 size = scale * new Vector2(tex.width * scale2D.Value.x,
										   tex.height * scale2D.Value.y);
		size.x = Mathf.Clamp(size.x, minLength, maxLength);
		size.y = Mathf.Clamp(size.y, minLength, maxLength);

		var tRect = GUILayoutUtility.GetRect(size.x, size.y);
		GUI.DrawTexture(tRect, tex, scaleMode, alphaBlend, 0, color.Value, 0, 0);
	}
	public static void DrawTexture(Texture tex,
								   float width = float.NaN, float height = float.NaN,
								   ScaleMode scaleMode = ScaleMode.StretchToFill,
								   bool alphaBlend = false,
								   Color? color = null)
	{
		if (!color.HasValue)
			color = Color.white;
		
		//Set width/height settings.
		var settings = new List<GUILayoutOption>() { GUILayout.ExpandWidth(float.IsNaN(width)),
													 GUILayout.ExpandHeight(float.IsNaN(height)) };
		if (!float.IsNaN(width))
			settings.Add(GUILayout.Width(width));
		if (!float.IsNaN(height))
			settings.Add(GUILayout.Height(height));

		var tRect = GUILayoutUtility.GetRect(new GUIContent(""), GUI.skin.label, settings.ToArray());
		GUI.DrawTexture(tRect, tex, scaleMode, alphaBlend, 0, color.Value, 0, 0);
	}


	public enum ListEditorResults
	{
		Nothing,
		DeleteElement,
		InsertElement,
		MoveElementUp,
		MoveElementDown,
	}
	/// <summary>
	/// Shows an editable list of items.
	/// Note that the list is edited automatically for you (if applicable),
	///     but the "PerformListAction()" method is provided publicly in case
	///     you have any parallel lists to the one being edited.
	/// You control whether elements can be added or removed inside the "elementGUI" function.
	/// If something was deleted from the list, that item is returned.
	/// </summary>
	/// <param name="foldValue">
	/// Stores the value of the foldout.
	/// </param>
	/// <param name="label">
	/// The label for this GUI, or null for no label.
	/// </param>
	/// <param name="list">
	/// The items being edited.
	/// </param>
	/// <param name="elementGUI">
	/// Does the GUI for a single element, and returns the result.
	/// Should include the various buttons like "delete", "insert here", etc.
	///     if you want to support that.
	/// </param>
	/// <param name="allowAddToEnd">
	/// If true, a button will be shown after all elements
	///     to insert an element at the end of the list.
	/// </param>
	/// <param name="result">
	/// Anything that happened to this GUI.
	/// </param>
	/// <param name="resultIndex">
	/// If "result" is something other than "Nothing",
	///     this contains the index that the result applies to.
	/// </param>
	/// <param name="makeNew">
	/// Makes a new instance to insert into the list.
	/// By default, just makes a "default(T)".
	/// </param>
	public static T ListEditor<T>(ref bool foldValue, string label,
					 			  IList<T> list,
					 			  Func<int, T, ListEditorResults> elementGUI,
								  bool allowAddToEnd,
								  out ListEditorResults result, out int resultIndex,
								  float tabSize = 20.0f,
								  Func<T> makeNew = null)
	{
		result = ListEditorResults.Nothing;
		resultIndex = -1;

		//The foldout.
		if (!Foldout(ref foldValue, label))
			return default(T);

		//The items.
		TabIn(tabSize);
		int i = 0;
		foreach (var item in list)
		{
			var itemResult = elementGUI(i, item);
			if (itemResult != ListEditorResults.Nothing && result == ListEditorResults.Nothing)
			{
				result = itemResult;
				resultIndex = i;
			}

			i += 1;
		}
		int count = i;
		TabOut();

		//The "add" button.
		if (allowAddToEnd)
		{
			BeginCompact(tabSize);
			if (GUILayout.Button("Add") && result == ListEditorResults.Nothing)
			{
				result = ListEditorResults.InsertElement;
				resultIndex = count;
			}
			EndCompact();
		}

		//Interpret the proper action to take.
		var returnVal = (result == ListEditorResults.DeleteElement) ?
							list[resultIndex] :
							default(T);
		PerformListAction(list, result, resultIndex, makeNew);

		return returnVal;
	}
	/// <summary>
	/// Performs the given action on the given list at the given index.
	/// </summary>
	/// <param name="makeNew">
	/// Makes a new element to insert into the list.
	/// By default, returns "default(T)".
	/// </param>
	public static void PerformListAction<T>(IList<T> list, ListEditorResults action, int i,
											Func<T> makeNew = null)
	{
		if (makeNew == null)
			makeNew = () => default(T);

		switch (action)
		{
			case ListEditorResults.Nothing: break;
			default: throw new NotImplementedException(action.ToString());

			case ListEditorResults.InsertElement:
				list.Insert(i, makeNew());
				break;
			case ListEditorResults.DeleteElement:
				list.RemoveAt(i);
				break;
				
			case ListEditorResults.MoveElementDown:
				var toMoveDown = list[i];
				list.RemoveAt(i);
				list.Insert(i + 1, toMoveDown);
				break;
			case ListEditorResults.MoveElementUp:
				var toMoveUp = list[i];
				list.RemoveAt(i);
				list.Insert(i - 1, toMoveUp);
				break;
		}
	}
}