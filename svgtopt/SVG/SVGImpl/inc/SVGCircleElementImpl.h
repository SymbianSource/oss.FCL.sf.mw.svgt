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
* Description:  SVG Implementation header file
 *
*/


#ifndef _INC_CSVGCIRCLEELEMENTIMPL_
#define _INC_CSVGCIRCLEELEMENTIMPL_

#include "SVGElementImpl.h"
#include "GfxEllipse2D.h"



class   CSvgDocumentImpl;


/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */
class CSvgCircleElementImpl : public CSvgElementImpl
    {
    public:

        // Constructor/deconstructor


		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgCircleElementImpl*NewL(  const TUint8 aElemID,
                                      CSvgDocumentImpl* aDoc );

		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        static CSvgCircleElementImpl*NewLC(  const TUint8 aElemID,
                                       CSvgDocumentImpl* aDoc );

	private:
		/**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            ConstructL(  const TUint8 aElemID  );


	public:
        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual                         ~CSvgCircleElementImpl();

        // From SVG DOM


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          Cx();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          Cy();


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TFloatFixPt                          R();

        // SVG Implementation


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetCx( TFloatFixPt aCx );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetCy( TFloatFixPt aCy );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            SetR( TFloatFixPt aR );


        // From MXmlElement


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                            SetAttributeL( const TDesC& aName,
                                                       const TDesC& aValue );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TBool                           DrawL( CGfx2dGc* aGc,
                                               CSvgElementImpl* aElement );


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            GetBBox( TGfxRectangle2D& aBbox );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        virtual void                    GetUnscaledBBox( TGfxRectangle2D& /*aBbox*/ );

        // From MXmlElementOpt API


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                            GetAttributeFloat( const TInt aNameId,
                                                           TFloatFixPt& aValue );

        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        TInt                            SetAttributeFloatL( const TInt aNameId,
                                                            const TFloatFixPt aValue );

        /**
         * Return wether an svg element is displayable, such as <rect>, <circle>, or
         * an abstract element, such as <g>, <animate>.
         *
         * @since 1.0
         * @param None
         * @return ETrue if element is viewable.
         */
        TBool               IsViewable() { return ETrue; }

    private:

        TGfxEllipse2D                   iCircle;


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
                                        CSvgCircleElementImpl( CSvgDocumentImpl* aDoc );
    public:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        MXmlElement*                    CloneL(MXmlElement* aParentElement);

		void							Print( TBool aIsEncodeOn );
		
    protected:


        /**
         * Need method description
         *
         * @since 1.0
         * @param
         * @return
         */
        void                            CopyL( CSvgCircleElementImpl* aDestElement );

    };

#endif
