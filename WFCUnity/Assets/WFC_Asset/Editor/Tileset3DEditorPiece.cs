using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;
using UnityEditor;


namespace WFC_CS.Editor
{
	/// <summary>
	/// A node in the "tree" of editor panes for the tileset editor.
	/// Provides functionality for tracking changes and saving/reverting them.
	/// </summary>
	public abstract class Tileset3DEditorPiece : IDisposable
	{
		public Tileset3D_EditorWindow OwnerWindow
		{
			get { return ownerWindow; }
			set
			{
				ownerWindow = value;

				foreach (var child in Children)
					child.OwnerWindow = value;
			}
		}
		private Tileset3D_EditorWindow ownerWindow;

		public bool IsDisabled
		{
			get { return isDisabled; }
			set
			{
				bool isDisabling = !isDisabled && value,
					 isEnabling = isDisabled && !value;
				isDisabled = value;

				if (isDisabled)
					OnDisabled();
				if (isEnabling)
					OnEnabled();
			}
		}
		private bool isDisabled = false;

		public bool HasUnsavedChanges { get; protected set; } = false;
		public bool HasUnsavedChanges_Recursive
		{
			get
			{
				return HasUnsavedChanges || AllChildren(true).Any(c => c.HasUnsavedChanges);
			}
		}

		public IReadOnlyList<Tileset3DEditorPiece> Children { get; private set; }
		public IEnumerable<Tileset3DEditorPiece> AllChildren(bool includeDisabled)
		{
			foreach (var child in Children)
			{
				yield return child;
				foreach (var deepChild in child.AllChildren(includeDisabled))
					yield return deepChild;
			}
		}


		/// <summary>
		/// Creates a new instance with the given children.
		/// </summary>
		public Tileset3DEditorPiece(IList<Tileset3DEditorPiece> children = null)
		{
			if (children == null)
				Children = new Tileset3DEditorPiece[0];
			else
				Children = children.ToList();
		}
		/// <summary>
		/// Default behavior: calls Dispose() on each child.
		/// </summary>
		public virtual void Dispose()
		{
			foreach (var child in Children)
				child.Dispose();
		}


		/// <summary>
		/// Saves this editor's changes to the edited item.
		/// Default behavior: calls "SaveChanges()" on all children,
		///     then calls "RevertChanges()" because
		///     a new copy of the data may have to be made after saving the current one.
		/// </summary>
		public virtual void SaveChanges()
		{
			foreach (var child in Children)
				child.SaveChanges();

			RevertChanges();
		}
		/// <summary>
		/// Removes this editor's changes to the edited item since it was last saved.
		/// Default behavior: calls "RevertChanges()" on all children,
		///     then sets "HasUnsavedChanges" to false.
		/// </summary>
		public virtual void RevertChanges()
		{
			foreach (var child in Children)
				child.RevertChanges();

			HasUnsavedChanges = false;
		}
		
		/// <summary>
		/// If there are any unsaved changes, asks the user and then potentially saves them.
		/// </summary>
		public ConfirmClosingDialog.Results ConfirmClosing(bool canCancel)
		{
			//First ask all children to close.
			//If any of them want to cancel, we will cancel.
			foreach (var child in Children)
			{
				var childResult = child.ConfirmClosing(canCancel);
				if (childResult == ConfirmClosingDialog.Results.Cancel)
					return ConfirmClosingDialog.Results.Cancel;
			}

			if (!HasUnsavedChanges)
				return ConfirmClosingDialog.Results.DontSave;

			var msg = "Do you want to save your changes to " + Description + "?";
			var result = ConfirmClosingDialog.Show(canCancel,msg);
			switch (result)
			{
				case ConfirmClosingDialog.Results.Cancel: break;

				case ConfirmClosingDialog.Results.Save:
					SaveChanges();
					break;

				case ConfirmClosingDialog.Results.DontSave:
					RevertChanges();
					break;

				default: throw new NotImplementedException(result.ToString());
			}
			return result;
		}

		/// <summary>
		/// Does the GUI for this part of the editor.
		/// Default behavior: calls DoGUILayout on each child
		///     (then takes ownership of their unsaved changes if TakeChildUnsavedChanges is true).
		/// </summary>
		public virtual void DoGUILayout()
		{
			foreach (var child in Children)
				child.DoGUILayout();

			if (TakeChildUnsavedChanges)
				foreach (var child in Children)
					if (child.HasUnsavedChanges)
					{
						child.HasUnsavedChanges = false;
						HasUnsavedChanges = true;
					}
		}


		/// <summary>
		/// Whether this editor piece should take ownership of its childrens' unsaved changes
		/// This can help prevent a ton of separate "Save your changes?" prompts
		///     when closing the window.
		/// Note that if enabled, the childrens' "SaveChanges()" methods will never get called.
		/// Default value: false.
		/// </summary>
		protected virtual bool TakeChildUnsavedChanges { get { return false; } }
		/// <summary>
		/// Used in the TryToClose() method. For example, a value of "tile 'MyTile'"
		///     results in the message "Do you want to save your changes to tile 'MyTile'?"
		/// </summary>
		public abstract string Description { get; }
		
		/// <summary>
		/// Creates a GUIBlock that automatically updates HasUnsavedChanges
		///     if any changes are detected.
		/// </summary>
		/// <param name="doOnChanged">
		/// Extra code to run if changes are detected.
		/// </param>
		protected GUIBlock TrackChanges(Action doOnChanged = null)
		{
			if (doOnChanged == null)
				doOnChanged = () => { };

			return GUIBlock.ChangeCheck(() =>
			{
				HasUnsavedChanges = true;
				doOnChanged();
			});
		}
		
		/// <summary>
		/// Called when this item is disabled.
		/// Default behavior: does nothing.
		/// </summary>
		protected virtual void OnDisabled() { }
		/// <summary>
		/// Called when this item is enabled.
		/// Default behavior: does nothing.
		/// </summary>
		protected virtual void OnEnabled() { }
	}
}
