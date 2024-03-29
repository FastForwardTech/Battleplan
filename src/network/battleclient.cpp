/****************************************************************************
**
** Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebSockets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "battleclient.h"
#include <QtCore/QDebug>
#include <QMessageBox>
#include <utility>

QT_USE_NAMESPACE

BattleClient::BattleClient(QObject *parent) :
	QObject(parent)
{
	mQuuid = QUuid::createUuid();
}

BattleClient::~BattleClient()
{
	m_webSocket.close();
}

void BattleClient::initializeState(State::GameState aState)
{
	state = aState;
	sendState();
}

void BattleClient::updateLocalState(State::GameState aState)
{
	state = aState;
}

void BattleClient::connectToServer(const QString code)
{
	emit closed();
	int port = convertCodeToPort(code);
	QString connectString = QString("ws://battleplan.fastforwardtech.net:%1").arg(port);
	m_url = QUrl(connectString);
	connect(&m_webSocket, &QWebSocket::connected, this, &BattleClient::onConnected);
	connect(&m_webSocket, SIGNAL(connected()), this, SIGNAL(connected()));
	connect(&m_webSocket, &QWebSocket::disconnected, this, &BattleClient::closed);
	m_webSocket.open(m_url);
}

void BattleClient::disconnect()
{
	m_webSocket.close();
}

void BattleClient::initializeMap(QByteArray mapData)
{
	mMapData = mapData;
	sendMapData();
}

void BattleClient::sendMapData()
{
	BattleMessage msg;
	msg.type = MAP;
	msg.data = mMapData;
	sendBattleMessage(msg);
}

void BattleClient::updateGridOffsetX(int aOffset)
{
	state.gridOffsetX = aOffset;
	sendState();
}

void BattleClient::updateGridOffsetY(int aOffset)
{
	state.gridOffsetY = aOffset;
	sendState();
}

void BattleClient::updateGridColor(QColor aColor)
{
	state.gridColor = aColor;
	sendState();
}

void BattleClient::updateGridStep(qreal aStep)
{
	state.gridStep = aStep;
	sendState();
}

void BattleClient::updatePlayers(QVector<Player*> aPlayers)
{
	state.players.clear();
	state.numPlayers = aPlayers.size();
	for (Player* pPlayer: aPlayers)
	{
		State::Player player;
		player.name = pPlayer->getName();
		player.x = pPlayer->getGridPos().x();
		player.y = pPlayer->getGridPos().y();
		player.red = pPlayer->color().red();
		player.green = pPlayer->color().green();
		player.blue = pPlayer->color().blue();
		player.maxHp = pPlayer->getMaxHitpoints();
		player.currHp = pPlayer->getCurrentHitpoints();
		player.conditions = pPlayer->getConditions();
		player.initiative = pPlayer->getInitiative();
		state.players.append(player);
	}
	sendState();
}

void BattleClient::addMarker(PositionMarker * aMarker)
{
	State::Marker marker;
	marker.x = aMarker->getGridPos().x();
	marker.y = aMarker->getGridPos().y();
	marker.valid = true;
	state.marker = marker;
	sendState();
	marker.valid = false;
	state.marker.valid = false;
}

void BattleClient::onConnected()
{
	connect(&m_webSocket, &QWebSocket::binaryMessageReceived, this, &BattleClient::onBinaryMessageReceived);
	m_webSocket.sendTextMessage("New client");
	connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &BattleClient::onTextMessageReceived);
}

void BattleClient::onBinaryMessageReceived(QByteArray data)
{	
	BattleMessage msg;
	msg = msg.deserialize(data);
	// this message was originally sent by this client; ignore it
	if (msg.source == mQuuid)
	{
		return;
	}
	// received message version does not match expected version; ignore it
	if (msg.msgVersion != version)
	{
		QMessageBox::warning(nullptr, "Version Mismatch", "Unexpected message format. Please ensure the same client versions are being used!");
		return;
	}
	fflush(stdout);
	if (msg.type == MAP)
	{
		// update map
		QImage img;
		QDataStream stream(msg.data);
		stream >> img;
		emit mapUpdateFromServer(img);
	}
	else if (msg.type == STATE)
	{
		// update state
		State::GameState state;
		state = state.deserialize(msg.data);
		emit stateUpdateFromServer(state);
	}
	else
	{
		printf("Unknown message type: %d\n", msg.type);
	}
}

void BattleClient::onTextMessageReceived(QString aMessage)
{
	if (aMessage == "New client")
	{
		sendMapData();
		sendState();
	}
}

State::GameState BattleClient::getState() const
{
	return state;
}

void BattleClient::sendState()
{
	BattleMessage msg;
	msg.type = STATE;
	msg.data = state.serialize();
	sendBattleMessage(msg);
}

int BattleClient::convertCodeToPort(QString code)
{
	int ret = 0;
	code = code.toLower();
	for (auto i = code.constBegin(); i < code.constEnd(); i++)
	{
		ushort unicode = i->unicode();
		int position = unicode - QChar('a').unicode() + 1;
		ret *= 10;
		ret += position;
	}
	return ret;
}

void BattleClient::sendBattleMessage(BattleClient::BattleMessage msg)
{
	msg.source = mQuuid;
	msg.msgVersion = version;
	m_webSocket.sendBinaryMessage(msg.serialize());
}
