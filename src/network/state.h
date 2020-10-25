#ifndef STATE_H
#define STATE_H

#include <QColor>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QString>
#include <QList>

namespace State {

struct Player {
	QString name;
	quint8 x;
	quint8 y;
	quint8 red;
	quint8 green;
	quint8 blue;
	quint8 maxHp;
	quint8 currHp;
	QVector<QString> conditions;

	friend QDataStream& operator <<(QDataStream &out, const Player &p)
	{
		return p.write(out);
	}

	friend QDataStream& operator >>(QDataStream &in, Player &p)
	{
		return p.read(in);
	}

	QDataStream& read(QDataStream &dataStream)
	{
		dataStream >> name;
		dataStream >> x;
		dataStream >> y;
		dataStream >> red;
		dataStream >> green;
		dataStream >> blue;
		dataStream >> maxHp;
		dataStream >> currHp;
		dataStream >> conditions;
		return dataStream;
	}

	QDataStream& write(QDataStream &dataStream) const
	{
		dataStream << name;
		dataStream << x;
		dataStream << y;
		dataStream << red;
		dataStream << green;
		dataStream << blue;
		dataStream << maxHp;
		dataStream << currHp;
		dataStream << conditions;
		return dataStream;
	}
};

struct GameState {
	quint8 gridOffsetX;
	quint8 gridOffsetY;
	quint8 gridStep;
	quint8 numPlayers;
	QVector<Player> players;

	QByteArray serialize()
	{
		QByteArray byteArray;

		QDataStream stream(&byteArray, QIODevice::WriteOnly);

		stream << gridOffsetX;
		stream << gridOffsetY;
		stream << gridStep;
		stream << numPlayers;
		stream << players;

		return byteArray;
	}

	GameState deserialize(const QByteArray& byteArray)
	{
		QDataStream stream(byteArray);
		GameState state;

		stream >> state.gridOffsetX
				>> state.gridOffsetY
				>> state.gridStep
				>> state.numPlayers
				>> state.players;
		return state;
	}

};
}

#endif // STATE_H
