#include "ControlPoint.h"

ControlPoint::ControlPoint()
{
}

ControlPoint::~ControlPoint()
{
}


char *ControlPoint::GetFirstDocumentItem( IXML_Document * doc,
                                 const char *item )
{
    IXML_NodeList *nodeList = NULL;
    IXML_Node *textNode = NULL;
    IXML_Node *tmpNode = NULL;

    char *ret = NULL;

    nodeList = ixmlDocument_getElementsByTagName( doc, ( char * )item );

    if( nodeList ) {
        if( ( tmpNode = ixmlNodeList_item( nodeList, 0 ) ) ) {
            textNode = ixmlNode_getFirstChild( tmpNode );

            ret = strdup( ixmlNode_getNodeValue( textNode ) );
        }
    }

    if( nodeList )
        ixmlNodeList_free( nodeList );
    return ret;
}
