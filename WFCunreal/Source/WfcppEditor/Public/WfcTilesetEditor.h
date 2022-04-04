#pragma once

#include "CoreMinimal.h"
#include "WfcTileset.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Widgets/Views/STileView.h"

class UWfcTileset;


//TODO: Use the module Editor/AdvancedPreviewScene

class IWfcTilesetEditor : public FAssetEditorToolkit
{
public:
	virtual UWfcTileset* GetAsset() const = 0;
	virtual void SetAsset(UWfcTileset* asset) = 0;
};


extern const FName WfcTileset_TabID_Properties, WfcTileset_TabID_FaceSelector,
                   WfcTileset_TabID_TileSelector, WfcTileset_TabID_TileViewer;

class WFCPPEDITOR_API FWfcTilesetEditor : public IWfcTilesetEditor
{
public:
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& tabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& tabManager) override;

	void InitWfcTilesetEditorEditor(const EToolkitMode::Type mode,
								    const TSharedPtr<class IToolkitHost>& initToolkitHost,
								    UWfcTileset* tileset);

	virtual ~FWfcTilesetEditor() override;

	//IToolkit interface:
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsPrimaryEditor() const override { return true; }

	//IWfcTilesetEditor interface:
	virtual UWfcTileset* GetAsset() const override { return tileset; }
	virtual void SetAsset(UWfcTileset* asset) override;

protected:
    //Generates the object picker for a tile.
    //By default, makes a picker that lets you select *any* kind of UObject.
    virtual TSharedRef<SWidget> MakeTileDataPicker(int tileID);

private:

	TSharedRef<SDockTab> GeneratePropertiesTab(const FSpawnTabArgs& args);
	TSharedRef<SDockTab> GenerateExperimentsTab(const FSpawnTabArgs& args);
    TSharedRef<SDockTab> GenerateFaceSelectorTab(const FSpawnTabArgs& args);
    TSharedRef<SDockTab> GenerateTileSelectorTab(const FSpawnTabArgs& args);
    TSharedRef<SDockTab> GenerateTileViewerTab(const FSpawnTabArgs& args);

	//Callbacks for face prototype data editors:
	FReply GenerateFacePrototypeData();
	FReply RemoveFacePrototypeData(int ID);
	TSharedRef<ITableRow> GenerateFacePrototypeUI(TSharedPtr<int> item,
												  const TSharedRef<STableViewBase>& owner);
    
    //Callbacks for tile data editors:
    FReply GenerateTileData();
    FReply RemoveTileData(int ID);
    TSharedRef<ITableRow> GenerateTileUI(TSharedPtr<int> item,
                                         const TSharedRef<STableViewBase>& owner);
	
	UWfcTileset* tileset;
	//Internal references to the asset data for editing in particular formats.
    TArray<TSharedPtr<int>> tilesetBuf_PrototypeIDs, tilesetBuf_TileIDs;

	TSharedPtr<SDockTab> propertiesTab, faceSelectorTab, tileSelectorTab;
	TSharedPtr<class IDetailsView> detailsView;
	TSharedPtr<SListView<TSharedPtr<int>>> faceSelector;
    TSharedPtr<STileView<TSharedPtr<int>>> tileSelector;


    //Declare an interface into a tileset tile's 'Data' property.
    DECLARE_DELEGATE_RetVal(UWfcTileset*, FTilesetGetter);
    class TileDataPropertyHandle : public IPropertyHandle
    {
    public:
        TileDataPropertyHandle(FTilesetGetter tilesetGetter, int id);

        //Interface implementation:
        virtual bool IsValidHandle() const override;
        virtual bool IsEditConst() const override;
        virtual bool IsEditable() const override;
        virtual const FFieldClass* GetPropertyClass() const override;
        virtual FProperty* GetProperty() const override;
        virtual FProperty* GetMetaDataProperty() const override;
        virtual bool HasMetaData(const FName& Key) const override;
        virtual const FString& GetMetaData(const FName& Key) const override;
        virtual bool GetBoolMetaData(const FName& Key) const override;
        virtual int32 GetIntMetaData(const FName& Key) const override;
        virtual float GetFloatMetaData(const FName& Key) const override;
        virtual UClass* GetClassMetaData(const FName& Key) const override;
        virtual void SetInstanceMetaData(const FName& Key, const FString& Value) override;
        virtual const FString* GetInstanceMetaData(const FName& Key) const override;
        virtual const TMap<FName, FString>* GetInstanceMetaDataMap() const override;
        virtual FText GetToolTipText() const override;
        virtual void SetToolTipText(const FText& ToolTip) override;
        virtual bool HasDocumentation() override;
        virtual FString GetDocumentationLink() override;
        virtual FString GetDocumentationExcerptName() override;
        virtual uint8* GetValueBaseAddress(uint8* Base) const override;
        virtual FPropertyAccess::Result GetValueAsFormattedString(FString& OutValue, EPropertyPortFlags PortFlags) const override;
        virtual FPropertyAccess::Result GetValueAsDisplayString(FString& OutValue, EPropertyPortFlags PortFlags) const override;
        virtual FPropertyAccess::Result GetValueAsFormattedText(FText& OutValue) const override;
        virtual FPropertyAccess::Result GetValueAsDisplayText(FText& OutValue) const override;
        virtual FPropertyAccess::Result SetValueFromFormattedString(const FString& InValue,
                                                                    EPropertyValueSetFlags::Type Flags) override;
        virtual void SetOnPropertyValueChanged(const FSimpleDelegate& InOnPropertyValueChanged) override;
        virtual void SetOnChildPropertyValueChanged(const FSimpleDelegate& InOnChildPropertyValueChanged) override;
        virtual void SetOnPropertyValuePreChange(const FSimpleDelegate& InOnPropertyValuePreChange) override;
        virtual void SetOnChildPropertyValuePreChange(const FSimpleDelegate& InOnChildPropertyValuePreChange) override;
        virtual void SetOnPropertyResetToDefault(const FSimpleDelegate& InOnPropertyResetToDefault) override;
        virtual FPropertyAccess::Result GetValue(float& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(double& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(bool& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(int8& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(int16& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(int32& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(int64& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(uint8& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(uint16& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(uint32& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(uint64& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(FString& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(FText& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(FName& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(FVector& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(FVector2D& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(FVector4& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(FQuat& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(FRotator& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(UObject*& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(const UObject*& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(FAssetData& OutValue) const override;
        virtual FPropertyAccess::Result GetValueData(void*& OutAddress) const override;
        virtual FPropertyAccess::Result GetValue(FProperty*& OutValue) const override;
        virtual FPropertyAccess::Result GetValue(const FProperty*& OutValue) const override;
        virtual FPropertyAccess::Result SetValue(const float& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const double& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const bool& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const int8& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const int16& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const int32& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const int64& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const uint8& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const uint16& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const uint32& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const uint64& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const FString& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const FText& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const FName& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const FVector& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const FVector2D& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const FVector4& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const FQuat& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const FRotator& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(UObject* const& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const UObject* const& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const FAssetData& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const TCHAR* InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(FProperty* const& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result SetValue(const FProperty* const& InValue, EPropertyValueSetFlags::Type Flags) override;
        virtual void NotifyPreChange() override;
        virtual void NotifyPostChange(EPropertyChangeType::Type ChangeType) override;
        virtual void NotifyFinishedChangingProperties() override;
        virtual FPropertyAccess::Result SetObjectValueFromSelection() override;
        virtual int32 GetNumPerObjectValues() const override;
        virtual FPropertyAccess::Result SetPerObjectValues(const TArray<FString>& PerObjectValues,
                                                           EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result GetPerObjectValues(TArray<FString>& OutPerObjectValues) const override;
        virtual FPropertyAccess::Result SetPerObjectValue(const int32 ObjectIndex, const FString& ObjectValue,
                                                          EPropertyValueSetFlags::Type Flags) override;
        virtual FPropertyAccess::Result GetPerObjectValue(const int32 ObjectIndex, FString& OutObjectValue) const override;
        virtual int32 GetIndexInArray() const override;
        virtual TSharedPtr<IPropertyHandle> GetChildHandle(FName ChildName, bool bRecurse) const override;
        virtual TSharedPtr<IPropertyHandle> GetChildHandle(uint32 Index) const override;
        virtual TSharedPtr<IPropertyHandle> GetParentHandle() const override;
        virtual TSharedPtr<IPropertyHandle> GetKeyHandle() const override;
        virtual FPropertyAccess::Result GetNumChildren(uint32& OutNumChildren) const override;
        virtual uint32 GetNumOuterObjects() const override;
        virtual void GetOuterObjects(TArray<UObject*>& OuterObjects) const override;
        virtual void ReplaceOuterObjects(const TArray<UObject*>& OuterObjects) override;
        virtual void GetOuterPackages(TArray<UPackage*>& OuterPackages) const override;
        virtual void EnumerateRawData(const EnumerateRawDataFuncRef& InRawDataCallback) override;
        virtual void EnumerateConstRawData(const EnumerateConstRawDataFuncRef& InRawDataCallback) const override;
        virtual void AccessRawData(TArray<void*>& RawData) override;
        virtual void AccessRawData(TArray<const void*>& RawData) const override;
        virtual TSharedPtr<IPropertyHandleArray> AsArray() override;
        virtual TSharedPtr<IPropertyHandleSet> AsSet() override;
        virtual TSharedPtr<IPropertyHandleMap> AsMap() override;
        virtual FText GetPropertyDisplayName() const override;
        virtual void SetPropertyDisplayName(FText InDisplayName) override;
        virtual void ResetToDefault() override;
        virtual bool DiffersFromDefault() const override;
        virtual FText GetResetToDefaultLabel() const override;
        virtual bool GeneratePossibleValues(TArray<TSharedPtr<FString>>& OutOptionStrings, TArray<FText>& OutToolTips,
                                            TArray<bool>& OutRestrictedItems) override;
        virtual void MarkHiddenByCustomization() override;
        virtual void MarkResetToDefaultCustomized(bool bCustomized) override;
        virtual void ClearResetToDefaultCustomized() override;
        virtual bool IsCustomized() const override;
        virtual bool IsResetToDefaultCustomized() const override;
        virtual FString GeneratePathToProperty() const override;
        virtual TSharedRef<SWidget> CreatePropertyNameWidget(const FText& NameOverride, const FText& ToolTipOverride,
                                                             bool bDisplayResetToDefault, bool bDisplayText, bool bDisplayThumbnail) const override;
        virtual TSharedRef<SWidget> CreatePropertyValueWidget(bool bDisplayDefaultPropertyButtons) const override;
        virtual TSharedRef<SWidget> CreateDefaultPropertyButtonWidgets() const override;
        virtual void CreateDefaultPropertyCopyPasteActions(FUIAction& OutCopyAction, FUIAction& OutPasteAction) const override;
        virtual void AddRestriction(TSharedRef<const FPropertyRestriction> Restriction) override;
        virtual bool IsRestricted(const FString& Value) const override;
        virtual bool IsRestricted(const FString& Value, TArray<FText>& OutReasons) const override;
        virtual bool GenerateRestrictionToolTip(const FString& Value, FText& OutTooltip) const override;
        virtual bool IsDisabled(const FString& Value) const override;
        virtual bool IsDisabled(const FString& Value, TArray<FText>& OutReasons) const override;
        virtual bool IsHidden(const FString& Value) const override;
        virtual bool IsHidden(const FString& Value, TArray<FText>& OutReasons) const override;
        virtual void SetIgnoreValidation(bool bInIgnore) override;
        virtual TArray<TSharedPtr<IPropertyHandle>> AddChildStructure(TSharedRef<FStructOnScope> ChildStructure) override;
        virtual bool CanResetToDefault() const override;
        virtual void ExecuteCustomResetToDefault(const FResetToDefaultOverride& OnCustomResetToDefault) override;
        virtual FName GetDefaultCategoryName() const override;
        virtual FText GetDefaultCategoryText() const override;
        //End interface implementation
    
    private:
        FTilesetGetter tilesetGetter;
        int ID;
    };
};