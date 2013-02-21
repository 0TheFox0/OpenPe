#ifndef STRINGCONVERTER_H
#define STRINGCONVERTER_H

#include <QObject>

class StringConverter : public QObject , public QString
{
	Q_OBJECT

public:
	StringConverter(QObject *parent);
	StringConverter();
	~StringConverter();
public slots:
	QString ToHexString(int,QString);
	QString ToMapleString(QString);
	QString ToLittleEndian(QString);

private:
	bool isHex(QString);
};

#endif // STRINGCONVERTER_H
