#pragma once

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailCustomNodeBuilder.h"
#include "IDetailGroup.h"
#include "SResetToDefaultMenu.h"


struct FWfcTilesetPropertiesBuilder_FacePrototypes : public IDetailCustomNodeBuilder
{
    FWfcTilesetPropertiesBuilder_FacePrototypes(TSharedRef<IPropertyHandle> propertyHandle)
        : propertyHandle(propertyHandle->AsMap()),
          propertyHandleBase(propertyHandle)
    {
        check(this->propertyHandle.IsValid());

        //Hook up a callback for when the map's size changes.
        auto numElementsChanged = FSimpleDelegate::CreateRaw(this, &FWfcTilesetPropertiesBuilder_FacePrototypes::OnCountChanged);
        this->propertyHandle->SetOnNumElementsChanged(numElementsChanged);

        propertyHandle->MarkHiddenByCustomization();
    }
    virtual ~FWfcTilesetPropertiesBuilder_FacePrototypes() override
    {
        FSimpleDelegate empty;
        propertyHandle->SetOnNumElementsChanged(empty);
    }

    virtual void SetOnRebuildChildren(FSimpleDelegate onRegenerateChildren) override { onRebuildChildren = onRegenerateChildren; }
    virtual void GenerateHeaderRowContent(FDetailWidgetRow& nodeRow) override
    {
        UE_LOG(LogWfcppEditor, Warning,
               TEXT("Header for %s"),
               *propertyHandleBase->GeneratePathToProperty());
        
        TSharedPtr<SHorizontalBox> contentBox;
        SAssignNew(contentBox, SHorizontalBox);
        contentBox->AddSlot() [
            propertyHandleBase->CreatePropertyValueWidget()
        ];

        FUIAction actCopy, actPaste;
        propertyHandleBase->CreateDefaultPropertyCopyPasteActions(actCopy, actPaste);

        //Set some metadata for this property row.
        nodeRow.FilterString(FText::FromString("Face Prototype"))
               .NameContent() [
                   propertyHandleBase->CreatePropertyNameWidget(FText::FromString("WFC Prop Name"),
                                                                FText::GetEmpty(),
                                                                true)
               ]
               .CopyAction(actCopy)
               .PasteAction(actPaste);

        TSharedPtr<SResetToDefaultMenu> resetToDefaultMenu;
        contentBox->AddSlot()
            .AutoWidth()
            .Padding(FMargin(2, 0, 0, 0))
        [
            SAssignNew(resetToDefaultMenu, SResetToDefaultMenu)
        ];
        resetToDefaultMenu->AddProperty(propertyHandleBase.ToSharedRef());
    }
    virtual void GenerateChildContent(IDetailChildrenBuilder& childrenBuilder) override
    {
        UE_LOG(LogWfcppEditor, Warning,
               TEXT("Body for %s"),
               *propertyHandleBase->GeneratePathToProperty());
        
        uint32 nChildren = 0;
        auto result = propertyHandle->GetNumElements(nChildren);
        check(result == FPropertyAccess::Success);

        for (uint32 i = 0; i < nChildren; ++i)
        {
            childrenBuilder.AddChildGroup(FName("A"), FText::FromString("B"))
                .AddWidgetRow()
                .NameContent() [
                    SNew(STextBlock)
                        .Text(FText::FromString("CC"))
                ]
                .ValueContent() [
                    SNew(STextBlock)
                        .Text(FText::FromString(FString::Printf(TEXT("DD%i"), i)))
                ];
            //TODO: How to do this with maps? Maybe with the callback? It seems like the API for Maps (and Sets) is missing features that Array has.
        }
    }
    
    virtual void Tick(float DeltaTime) override { }
    virtual bool RequiresTick() const override { return false; }
    virtual bool InitiallyCollapsed() const override { return false; }
    virtual FName GetName() const override { return propertyHandleBase->GetProperty()->GetFName(); }

private:

    TSharedPtr<IPropertyHandleMap> propertyHandle;
    TSharedPtr<IPropertyHandle> propertyHandleBase;
    FSimpleDelegate onRebuildChildren;

    void OnCountChanged()
    {
        bool wasCalled = onRebuildChildren.ExecuteIfBound();
    }
};
