#ifndef I3_SHOVEL_ARTIST_DELEGATE_H
#define I3_SHOVEL_ARTIST_DELEGATE_H

#include <QStyledItemDelegate>

class ArtistDelegate : public QStyledItemDelegate{

	Q_OBJECT;

public:
	ArtistDelegate( QObject* parent = NULL );

	virtual QWidget* createEditor( QWidget *parent, const QStyleOptionViewItem &option,
                                   const QModelIndex &index ) const;

	virtual void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem &option,
	                                   const QModelIndex &index) const;

};


#endif /* I3_SHOVEL_ARTIST_DELEGATE_H */
