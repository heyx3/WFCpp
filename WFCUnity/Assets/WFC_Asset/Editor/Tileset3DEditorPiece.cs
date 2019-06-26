using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WFC_CS.Editor
{
	/// <summary>
	/// A node in the "tree" of editor panes for the tileset editor.
	/// Provides functionality for tracking changes and saving/reverting them.
	/// </summary>
	public abstract class Tileset3DEditorPiece : IDisposable
	{
		public bool HasUnsavedChanges { get; protected set; } = false;
		public bool HasUnsavedChanges_Recursive
		{
			get
			{
				return HasUnsavedChanges || AllChildren.Any(c => c.HasUnsavedChanges);
			}
		}

		public List<Tileset3DEditorPiece> Children { get; protected set; } = new List<Tileset3DEditorPiece>();
		public IEnumerable<Tileset3DEditorPiece> AllChildren
		{
			get
			{
				foreach (var child in Children)
				{
					yield return child;
					foreach (var deepChild in child.AllChildren)
						yield return deepChild;
				}
			}
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
		/// Default behavior: calls Dispose() on each child.
		/// </summary>
		public virtual void Dispose()
		{
			foreach (var child in Children)
				child.Dispose();
		}

		/// <summary>
		/// Does the GUI for this part of the editor.
		/// Default behavior: calls DoGUILayout on each child.
		/// </summary>
		public virtual void DoGUILayout()
		{
			foreach (var child in Children)
				child.DoGUILayout();
		}


		/// <summary>
		/// Used in the TryToClose() method. For example, a value of "tile 'MyTile'"
		///     results in the message "Do you want to save your changes to tile 'MyTile'?"
		/// </summary>
		protected abstract string Description { get; }
		
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
	}
}
