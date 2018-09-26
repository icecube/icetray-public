#ifndef I3_SHOVEL_SAVEFRAMEDIALOG_H
#define I3_SHOVEL_SAVEFRAMEDIALOG_H

#include <QFileDialog>
#include <QCheckBox>
#include <QLineEdit>
#include <set>

class SaveFrameDialog: public QFileDialog {
	Q_OBJECT;

	QLineEdit* lineedit_;
	QCheckBox* checkbox_;

public:
	SaveFrameDialog( QWidget* parent );
	~SaveFrameDialog();

	QString getSaveFileName() const;
	std::set<unsigned> getIndices() const;
	bool saveParents() const { return checkbox_->isChecked(); }
};

#endif
