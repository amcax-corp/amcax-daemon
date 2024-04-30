#include "IntValidator.h"

IntValidator::IntValidator(int bottom, int top, QObject* parent) :QIntValidator(bottom, top, parent)
{
}

QValidator::State IntValidator::validate(QString& input, int& pos) const
{
	const QValidator::State state = QIntValidator::validate(input, pos);

	if (state == QValidator::Intermediate)
	{
		int i = input.toInt();

		if (i <= bottom())
		{
			return QValidator::Invalid;
		}
	}

	return state;
}