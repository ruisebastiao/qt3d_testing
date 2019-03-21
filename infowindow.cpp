#include "infowindow.h"

InfoWindow::InfoWindow(QQuickItem *parent)
{
    setParentItem( parent );
    setAntialiasing( true );
    setFlag( QQuickItem::ItemHasContents, true );
}
