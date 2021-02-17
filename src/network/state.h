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
	quint8 initiative;
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
		dataStream >> initiative;
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
		dataStream << initiative;
		dataStream << conditions;
		return dataStream;
	}
};

struct Marker {

	quint8 x;
	quint8 y;
	bool valid;

	friend QDataStream& operator <<(QDataStream &out, const Marker &p)
	{
		return p.write(out);
	}

	friend QDataStream& operator >>(QDataStream &in, Marker &p)
	{
		return p.read(in);
	}

	QDataStream& read(QDataStream &dataStream)
	{
		dataStream >> x;
		dataStream >> y;
		dataStream >> valid;
		return dataStream;
	}

	QDataStream& write(QDataStream &dataStream) const
	{
		dataStream << x;
		dataStream << y;
		dataStream << valid;
		return dataStream;
	}
};

struct GameState {
	quint8 gridOffsetX;
	quint8 gridOffsetY;
	QColor gridColor;
	double gridStep;
	quint8 numPlayers;
	QVector<Player> players;
	Marker marker;

	QByteArray serialize()
	{
		QByteArray byteArray;

		QDataStream stream(&byteArray, QIODevice::WriteOnly);

		stream << gridOffsetX;
		stream << gridOffsetY;
		stream << gridColor;
		stream << gridStep;
		stream << numPlayers;
		stream << players;
		stream << marker;

		return byteArray;
	}

	GameState deserialize(const QByteArray& byteArray)
	{
		QDataStream stream(byteArray);
		GameState state;

		stream >> state.gridOffsetX
				>> state.gridOffsetY
				>> state.gridColor
				>> state.gridStep
				>> state.numPlayers
				>> state.players
				>> state.marker;
		return state;
	}

};
}

#endif // STATE_H
