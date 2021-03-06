/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  SVG Implementation source file
 *
*/


#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#include "SVGTitleElementImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSchemaData.h"

#include "GfxAffineTransform.h"


// ==========================================================================
// Need method description
// ==========================================================================
CSvgTitleElementImpl* CSvgTitleElementImpl::NewL( const TUint8 aElemID,
                                                  CSvgDocumentImpl* aDoc )
    {
    CSvgTitleElementImpl*   self    = new ( ELeave )
                                      CSvgTitleElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );
    CleanupStack::Pop();

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgTitleElementImpl* CSvgTitleElementImpl::NewLC( const TUint8 aElemID,
                                                   CSvgDocumentImpl* aDoc )
    {
    CSvgTitleElementImpl*   self    = new ( ELeave )
                                      CSvgTitleElementImpl( aDoc );
    CleanupStack::PushL( self );
    self->ConstructL( aElemID );

    return self;
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgTitleElementImpl::~CSvgTitleElementImpl()
    {
    	if ( iText )
        {
        delete iText;
        iText = NULL;
        }
    }

// *******************************************************
// From MXmlElement

// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgTitleElementImpl::GetAttributeDes( const TInt aNameId, TPtrC16& aValue )
    {
    switch ( aNameId )
        {
        case KAtrCdata:
        aValue.Set( iText->Ptr() );
        break;
        default:
        return CSvgElementImpl::GetAttributeDes( aNameId, aValue );
        }
    return KErrNone;
    }
    
// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgTitleElementImpl::SetAttributeDesL( const TInt aNameId,
                                            const TDesC& aValue )
    {
    switch ( aNameId )
        {
        case KAtrCdata:
        SetTextL( aValue );
        break;

        default:
        return CSvgElementImpl::SetAttributeDesL( aNameId, aValue );
        }

    return KErrNone;
    }
    
// ==========================================================================
// Need method description
// ==========================================================================
TInt CSvgTitleElementImpl::SetAttributeL( const TDesC& aName,
                                          const TDesC& aValue )
    {

    if ( this->SetLangSpaceAttributeL( aName, aValue ) )
        {
        return KErrNone;
        }

    /*    if(this->SetClassName(aName, aValue))
        {
            return KErrNone;
        }
    */
  

    if ( this->SetTestAttributeL( aName, aValue ) )
        {
        return KErrNone;
        }

    if ( SetIdandXmlbaseL( aName, aValue ) )
        {
        return KErrNone;
        }

    return KErrNone;
    }

// ==========================================================================
// xml:space is an inheritable attribute which can have one of two values:
//
// default (the initial/default value for xml:space) - When xml:space="default":
//      - Remove all newline characters.
//      - Convert all tab characters into space characters.
//      - Strip off all leading and trailing space characters.
//      - All contiguous space characters will be consolidated.
//
// preserve - When xml:space="preserve" :
//      - Convert all newline and tab characters into space characters.
// ==========================================================================
void CSvgTitleElementImpl::SetTextL( const TDesC& aText )
    {
    _LIT( KPreserve, "preserve" );

    if ( iText )
        {
        delete iText;
        iText = NULL;
        }

    iText = aText.AllocL();
    TPtr textDes = iText->Des();

    // default
    if ( XMLSpace() != KPreserve )
        {
        for (TInt i = textDes.Length() - 1; i >= 0; i--)
        {
        	if (textDes[i] == '\n' || textDes[i] == '\r')
        	{
        	textDes.Delete(i,1);
        	} 
        }
        textDes.TrimRight();
        textDes.TrimLeft();
        }
    // preserve
    else
        {
        TInt textDesLength = textDes.Length();
        for ( TInt i = 0; i < textDesLength; i++ )
            {
            // ms-dos carriage return contains two characters: 13, 10
            if ( i + 1 < textDes.Length() && (TInt)textDes[i] == 13 && (TInt)textDes[i+1] == 10 )
                {
                textDes[i] = ' ';
                textDes.Delete( i+1, 1 );
                }
            else if ( TChar(textDes[i]).IsSpace() )
                {
                textDes[i] = ' ';
                }
            }
        }
    }
    
// *******************************************************
// From CSvgElementImpl

// perform a deep clone of this object
// ==========================================================================
// Need method description
// ==========================================================================
MXmlElement* CSvgTitleElementImpl::CloneL(MXmlElement* aParentElement)
    {
    CSvgTitleElementImpl* newElement = CSvgTitleElementImpl::NewL(  this->ElemID(), ((CSvgDocumentImpl*)iOwnerDocument) );

		CleanupStack::PushL(newElement);
		newElement->iParentNode = aParentElement;
    // copy everything over
    this->CopyL(newElement);
		CleanupStack::Pop();
    return newElement;
    	
    }

// ==========================================================================
// perform a deep copy of this object
// ==========================================================================
void CSvgTitleElementImpl::CopyL( CSvgTitleElementImpl* aDestElement )
    {
    	if(aDestElement)
    	{

    	// copy stuff from superclass
    	this->CSvgElementImpl::CopyL(aDestElement);

			if(this->iText)
			{
     		delete aDestElement->iText;
		 		aDestElement->iText= NULL;
		 		aDestElement->iText= (this->iText)->AllocL();
			}

    	// copy the reference to idoc (CSvgDocumentImpl)
    	aDestElement->iOwnerDocument = this->iOwnerDocument;
    	}
    }
    
// ==========================================================================
// Need method description
// ==========================================================================
TBool CSvgTitleElementImpl::DrawL( CGfx2dGc* /* aGc */,
                                   CSvgElementImpl* /* aElement */ )
    {
    return EFalse;
    }


// *******************************************************
// Private

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgTitleElementImpl::ConstructL( const TUint8 aElemID )
    {
    CSvgElementImpl::InitializeL( aElemID );
    
    iText = HBufC::NewL( 2 );
    }

// ==========================================================================
// Need method description
// ==========================================================================
CSvgTitleElementImpl::CSvgTitleElementImpl( CSvgDocumentImpl* aDoc )
    {
    	SetOwnerDocument( aDoc );
    }

// ==========================================================================
// Need method description
// ==========================================================================
void CSvgTitleElementImpl::Print( TBool aIsEncodeOn )
	{
		if (!aIsEncodeOn)
		{
			#ifdef _DEBUG
			RDebug::Printf("<title>");
			#endif
		}
	}

